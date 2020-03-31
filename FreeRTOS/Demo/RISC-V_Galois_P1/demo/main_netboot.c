/* Standard includes. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <elf.h>

/* FreeRTOS  includes. */
#include "FreeRTOS.h"
#include "task.h"

/* IP stack includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "NetworkInterface.h"

/* Application includes */
#include "uart.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define STR(x) #x
#define XSTR(x) STR(x)

#define CONCAT(x, y) x ## y
#define XCONCAT(x, y) CONCAT(x, y)
#define CONCAT3(x, y, z) x ## y ## z
#define XCONCAT3(x, y, z) CONCAT3(x, y, z)

typedef XCONCAT3(Elf, __riscv_xlen, _Half) Elf_Half;
typedef XCONCAT3(Elf, __riscv_xlen, _Word) Elf_Word;
typedef XCONCAT3(Elf, __riscv_xlen, _Addr) Elf_Addr;
typedef XCONCAT3(Elf, __riscv_xlen, _Off) Elf_Off;
typedef XCONCAT3(Elf, __riscv_xlen, _Ehdr) Elf_Ehdr;
typedef XCONCAT3(Elf, __riscv_xlen, _Phdr) Elf_Phdr;

#define ELFCLASSXLEN XCONCAT(ELFCLASS, __riscv_xlen)

#define STAGING_ADDR 0xE0000000UL
#define STAGING_BUFS_ALIGN 16

#define TFTP_OP_RRQ   1
#define TFTP_OP_WRQ   2
#define TFTP_OP_DATA  3
#define TFTP_OP_ACK   4
#define TFTP_OP_ERROR 5
#define TFTP_OP_OACK  6

#define TFTP_MAX_BLKSIZE  1024
#define TFTP_MAX_WINSIZE  16
#define TFTP_MAX_RETRIES  5
#define TFTP_MAX_RECVSIZE (sizeof(struct tftp_data) + TFTP_MAX_WINSIZE
#define TFTP_TIMEOUT_MS   5000

struct tftp_client_state
{
	Socket_t *sock;
	struct freertos_sockaddr dstaddr;
	socklen_t dstaddrlen;
	char *winbuf;
	void *recvpacket;
	uint16_t winprev;
	uint16_t winstart;
	uint16_t winsize;
	uint16_t winbits;
	uint16_t blksize;
	uint16_t lastblock;
	uint16_t lastsize;
	uint8_t retries;
	uint8_t havetid : 1;
	uint8_t pastrrq : 1;
};

struct tftp_header
{
	uint16_t op;
};

struct tftp_xrq
{
	struct tftp_header header;
	char request[];
};

struct tftp_data
{
	struct tftp_header header;
	uint16_t block;
	char data[];
};

struct tftp_ack
{
	struct tftp_header header;
	uint16_t block;
};

struct tftp_error
{
	struct tftp_header header;
	uint16_t code;
	char message[];
};

struct tftp_oack
{
	struct tftp_header header;
	char data[];
};

struct load_command
{
	char *src;
	char *dst;
	size_t copysz;
	size_t zerosz;
};

/* Must be in .data; startup code writes to it early before zeroing BSS! */
__attribute__((section(".data")))
size_t xDtbAddr;

extern char netboot_load_trampoline_start[];
extern char netboot_load_trampoline_end[];
typedef void (*netboot_load_trampoline_type)(size_t, size_t, size_t, struct load_command *, size_t, bool) __attribute__((noreturn));

/* The default IP and MAC address used by the demo.  The address configuration
defined here will be used if ipconfigUSE_DHCP is 0, or if ipconfigUSE_DHCP is
1 but a DHCP server could not be contacted.  See the online documentation for
more information. */
static const uint8_t ucIPAddress[4] = {configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3};
static const uint8_t ucNetMask[4] = {configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3};
static const uint8_t ucGatewayAddress[4] = {configGATEWAY_ADDR0, configGATEWAY_ADDR1, configGATEWAY_ADDR2, configGATEWAY_ADDR3};
static const uint8_t ucDNSServerAddress[4] = {configDNS_SERVER_ADDR0, configDNS_SERVER_ADDR1, configDNS_SERVER_ADDR2, configDNS_SERVER_ADDR3};

/* Default MAC address configuration.  The demo creates a virtual network
connection that uses this MAC address by accessing the raw Ethernet data
to and from a real network connection on the host PC.  See the
configNETWORK_INTERFACE_TO_USE definition for information on how to configure
the real network connection to use. */
const uint8_t ucMACAddress[6] = {configMAC_ADDR0, configMAC_ADDR1, configMAC_ADDR2, configMAC_ADDR3, configMAC_ADDR4, configMAC_ADDR5};

void main_netboot(void);
static void prvShellTask(void *pvParameters);
static void prvLoadAndBoot(int n, char **names, char **bufs, bool halt);
static int prvTftpReceive(const char *host, uint16_t port, const char *name, char *buf, size_t *size);

void main_netboot(void)
{

	printf("\r\n\r\n");
	printf("GFEBoot RV" XSTR(__riscv_xlen) "\r\n");
	printf("\r\n");
	printf("   _____ ______ ______ ____              _   \r\n");
	printf("  / ____|  ____|  ____|  _ \\            | |  \r\n");
	printf(" | |  __| |__  | |__  | |_) | ___   ___ | |_ \r\n");
	printf(" | | |_ |  __| |  __| |  _ < / _ \\ / _ \\| __|\r\n");
	printf(" | |__| | |    | |____| |_) | (_) | (_) | |_ \r\n");
	printf("  \\_____|_|    |______|____/ \\___/ \\___/ \\__|\r\n");
	printf("\r\n\r\n");

	FreeRTOS_IPInit(ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress);
}

void vApplicationIPNetworkEventHook(eIPCallbackEvent_t eNetworkEvent)
{
	static BaseType_t xTasksAlreadyCreated = pdFALSE;

	if (eNetworkEvent == eNetworkUp)
	{
		if (xTasksAlreadyCreated == pdFALSE)
		{
			xTaskCreate(prvShellTask, "Shell", configMINIMAL_STACK_SIZE * 10, NULL, tskIDLE_PRIORITY + 1, NULL);

			xTasksAlreadyCreated = pdTRUE;
		}
	}
}

/* Shell implementation */

static void prvShellCommandBoot(int argc, char **argv)
{
	char *bufs[2];
	size_t argi = 1;
	bool halt = false;
        unsigned long port = 69;

	if ((size_t)argc > argi && strcmp(argv[argi], "-h") == 0)
	{
		halt = true;
		++argi;
	}

        if ((size_t)argc > argi && strcmp(argv[argi], "-p") == 0)
        {
		++argi;
		if ((size_t)argc > argi)
		{
			errno = 0;
			port = strtoul(argv[argi], NULL, 10);
			if (port == 0 && errno != 0)
			{
				printf("Error: port must be a number\r\n");
				return;
			}
			if (port > UINT16_MAX)
			{
				printf("Error: port out of range\r\n");
				return;
			}
			++argi;
		}
        }

	if ((size_t)argc < argi + 2)
	{
		printf("Error: too few arguments\r\n");
		printf("Usage: boot [-h] [-p <port>] <host> <file> [file]\r\n");
		return;
	}

	if ((size_t)argc > argi + 1 + ARRAY_SIZE(bufs))
	{
		printf("Error: too many arguments\r\n");
		printf("Usage: boot [-h] [-p <port>] <host> <file> [file]\r\n");
		return;
	}

	const char *host = argv[argi];
	char *staging = (char *)(uintptr_t)STAGING_ADDR;
	for (int i = argi + 1; i < argc; ++i)
	{
		size_t size;
		printf("Requesting %s\r\n", argv[i]);
		if (prvTftpReceive(host, port, argv[i], staging, &size))
			return;

		bufs[i - argi - 1] = staging;
		staging += size + (-size % STAGING_BUFS_ALIGN);
	}

	printf("Booting\r\n");
	prvLoadAndBoot(argc - argi - 1, argv + argi + 1, bufs, halt);
}

static void prvShellCommandHelp(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	printf("Supported commands:\r\n");
	printf("    boot [-h] [-p <port>] <host> <file> [file]\r\n");
	printf("                              Load and boot the given file(s) via TFTP\r\n");
	printf("                              Optionally halts just before jumping\r\n");
	printf("    help                      Display this message\r\n");
	printf("    ifconfig                  Display network config\r\n");
}

static void prvShellCommandIfconfig(int argc, char **argv)
{
	uint32_t ulIPAddress, ulNetMask, ulGatewayAddress, ulDNSServerAddress;
	char cBuffer[16];

	if (argc > 1)
	{
		printf("Error: unexpected argument: %s\r\n", argv[1]);
		printf("Usage: %s\r\n", argv[0]);
		return;
	}

	FreeRTOS_GetAddressConfiguration(&ulIPAddress, &ulNetMask, &ulGatewayAddress, &ulDNSServerAddress);
	FreeRTOS_inet_ntoa(ulIPAddress, cBuffer);
	printf("IP Address: %s\r\n", cBuffer);

	FreeRTOS_inet_ntoa(ulNetMask, cBuffer);
	printf("Subnet Mask: %s\r\n", cBuffer);

	FreeRTOS_inet_ntoa(ulGatewayAddress, cBuffer);
	printf("Gateway Address: %s\r\n", cBuffer);

	FreeRTOS_inet_ntoa(ulDNSServerAddress, cBuffer);
	printf("DNS Server Address: %s\r\n", cBuffer);
}

struct
{
	const char *name;
	void (*func)(int, char **);
} xShellCommands[] =
{
	{ "boot", prvShellCommandBoot },
	{ "help", prvShellCommandHelp },
	{ "ifconfig", prvShellCommandIfconfig },
};

static void prvShellCommand(char *command)
{
	char *argv[16];
	int argc = 0;
	char *start = NULL;
	for (; *command; ++command)
	{
		if (*command == ' ' || *command == '\t')
		{
			if (start)
			{
				if (argc == ARRAY_SIZE(argv))
					break;
				*command = '\0';
				argv[argc++] = start;
				start = NULL;
			}
		}
		else if (!start)
		{
			start = command;
		}
	}
	if (start && (size_t)argc < ARRAY_SIZE(argv))
	{
		argv[argc++] = start;
		start = NULL;
	}

	if (start)
	{
		printf("Error: argument list too long\r\n");
		return;
	}

	if (argc == 0)
		return;

	for (size_t i = 0; i < ARRAY_SIZE(xShellCommands); ++i)
	{
		if (!strcmp(xShellCommands[i].name, argv[0]))
			return xShellCommands[i].func(argc, argv);
	}

	printf("Error: unknown command: %s\r\n", argv[0]);
	printf("Type 'help' to see a list of supported commands\r\n");
}

static void prvShellPrompt(void)
{
	printf("> ");
}

static void prvShellTask(void *pvParameters)
{
	(void)pvParameters;

	char buf[1024];
	int len = 0;
	bool overflow = false;
	bool nonprintable = false;
	prvShellPrompt();
	for (;;)
	{
		int n = uart0_rxbuffer(buf + len, sizeof(buf)-1-len);
		if (n == 0)
		{
			msleep(1);
			continue;
		}
		buf[len + n] = 0;
		char *command = buf;
		for (int i = 0; i < n; ++i)
		{
			switch (buf[len + i]) {
			/* Ctrl-C */
			case '\003':
				printf("\r\n");
				prvShellPrompt();
				len += i + 1;
				n -= i + 1;
				i = -1;
				command = buf + len;
				continue;
			/* Backspace */
			case '\010':
			{
				int delete;
				if (command == buf + len + i)
				{
					delete = 1;
				}
				else
				{
					printf("\010 \010");
					delete = 2;
				}
				memmove(buf + len + i, buf + len + i + delete, n - i);
				n -= delete;
				i -= delete;
				continue;
			}
			/* Delete */
			case '\177':
				memmove(buf + len + i, buf + len + i + 1, n - i);
				n -= 1;
				i -= 1;
				continue;
			/* Carriage Return */
			case '\r':
				printf("\r\n");
				buf[len + i] = 0;
				if (overflow)
				{
					printf("Error: character limit exceeed\r\n");
					overflow = false;
					nonprintable = false;
				}
				else if (nonprintable)
				{
					printf("Error: command contains unsupported control characters\r\n");
					overflow = false;
					nonprintable = false;
				}
				else
				{
					prvShellCommand(command);
				}
				prvShellPrompt();
				len += i + 1;
				n -= i + 1;
				i = -1;
				command = buf + len;
				continue;
			default:
				/* Unsupported non-printable characters */
				if (buf[len + i] != '\t' && buf[len + i] < '\040')
				{
					nonprintable = true;
					printf("^%c", buf[len + i] | '\100');
					continue;
				}
				/* Fallthrough */
				/* Tab */
			case '\t':
				putchar(buf[len + i]);
				break;
			}
		}
		len += n;
		if (command == buf && len == sizeof(buf)-1)
		{
			overflow = true;
		}

		if (overflow || nonprintable)
		{
			/*
			 * Empty the buffer if we are in the middle of a command with
			 * errors already found.
			 */
			len = 0;
		}
		else
		{
			len = (buf + len) - command;
			memmove(buf, command, len);
		}
	}
}

/* ELF loader */

static BaseType_t prvGetLoadCommands(struct load_command **commands,
		char *name, char *buf, size_t *pentry)
{
	Elf_Ehdr *ehdr = (Elf_Ehdr *)(uintptr_t)buf;
	if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
	    ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
	    ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
	    ehdr->e_ident[EI_MAG2] != ELFMAG2)
	{
		printf("Error: %s: not an ELF file\r\n", name);
		return 1;
	}

	if (ehdr->e_ident[EI_CLASS] != ELFCLASSXLEN)
	{
		printf("Error: %s: wrong ELF class: %d\r\n", name,
		       ehdr->e_ident[EI_CLASS]);
		return 1;
	}

	if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB)
	{
		printf("Error: %s: wrong ELF endianness: %d\r\n", name,
		       ehdr->e_ident[EI_DATA]);
		return 1;
	}

	if (ehdr->e_ident[EI_VERSION] != EV_CURRENT)
	{
		printf("Error: %s: wrong ELF version: %d\r\n", name,
		       ehdr->e_ident[EI_VERSION]);
		return 1;
	}

	if (ehdr->e_type != ET_EXEC && ehdr->e_type != ET_DYN)
	{
		printf("Error: %s: wrong ELF type: %d\r\n", name,
		       ehdr->e_type);
		return 1;
	}

	if (ehdr->e_machine != EM_RISCV)
	{
		printf("Error: %s: wrong ELF version: %d\r\n", name,
		       ehdr->e_ident[EI_VERSION]);
		return 1;
	}

	if (ehdr->e_phentsize != sizeof(Elf_Phdr))
	{
		printf("Error: %s: wrong e_phentsize: %d\r\n", name,
		       ehdr->e_phentsize);
		return 1;
	}

	Elf_Phdr *phdrs = (Elf_Phdr *)(uintptr_t)(buf + ehdr->e_phoff);
	size_t entry = 0;
	for (int i = ehdr->e_phnum - 1; i >= 0; --i)
	{
		if (phdrs[i].p_type != PT_LOAD)
			continue;

		if (phdrs[i].p_paddr < 0x80000000 ||
		    phdrs[i].p_paddr + phdrs[i].p_memsz > 0xE0000000)
		{
			printf("Error: %s: PT_LOAD segment %d not in bounds"
			       " (0x%08lx - 0x%08lx)\r\n", name, i,
			       (unsigned long)phdrs[i].p_paddr,
			       (unsigned long)phdrs[i].p_paddr + phdrs[i].p_memsz);
			return 1;
		}

		--*commands;
		(*commands)->src = buf + phdrs[i].p_offset;
		(*commands)->dst = (char *)(uintptr_t)phdrs[i].p_paddr;
		(*commands)->copysz = phdrs[i].p_filesz;
		(*commands)->zerosz = phdrs[i].p_memsz - phdrs[i].p_filesz;

		if (ehdr->e_entry >= phdrs[i].p_vaddr &&
		    ehdr->e_entry < phdrs[i].p_vaddr + phdrs[i].p_memsz)
			entry = phdrs[i].p_paddr + (ehdr->e_entry - phdrs[i].p_vaddr);
	}

	if (pentry)
	{
		if (!entry)
		{
			printf("Error: %s: entry point %08lx not in any segments\r\n",
			       name, (unsigned long)ehdr->e_entry);
			return 1;
		}

		*pentry = entry;
	}

	return 0;
}

static void prvLoadAndBoot(int n, char **names, char **bufs, bool halt)
{
	extern size_t xDtbAddr;
	struct load_command *commands =
		(struct load_command *)(uintptr_t)STAGING_ADDR;
	--commands;
	commands->src = NULL;
	commands->dst = NULL;
	commands->copysz = 0;
	commands->zerosz = 0;

	size_t entry;
	for (int i = n - 1; i >= 0; --i)
	{
		size_t *pentry = i == 0 ? &entry : NULL;
		if (prvGetLoadCommands(&commands, names[i], bufs[i], pentry))
			return;
	}

	printf("Taking down network interface\r\n");
	xNetworkInterfaceDestroy();

	size_t load_trampoline_size =
		netboot_load_trampoline_end - netboot_load_trampoline_start;
	netboot_load_trampoline_type load_trampoline =
		(netboot_load_trampoline_type)((uintptr_t)commands - load_trampoline_size);
	memcpy((char *)load_trampoline, netboot_load_trampoline_start,
	       load_trampoline_size);
	__asm__ __volatile__ ("fence.i" ::: "memory");
	load_trampoline(0, xDtbAddr, 0, commands, entry, halt);
}

/* TFTP implementation */

static void prvTftpTerminate(struct tftp_client_state *state, uint16_t code, char *message)
{
	if (state->recvpacket)
	{
		FreeRTOS_ReleaseUDPPayloadBuffer(state->recvpacket);
		state->recvpacket = NULL;
	}

	size_t mlen = strlen(message);
	size_t plen = sizeof(struct tftp_error) + mlen + 1;
	struct tftp_error *packet = pvPortMalloc(plen);
	memset(packet, 0, sizeof(*packet));
	packet->header.op = FreeRTOS_htons(TFTP_OP_ERROR);
	packet->code = FreeRTOS_htons(code);
	strcpy(packet->message, message);
	FreeRTOS_sendto(state->sock, packet, plen, 0, &state->dstaddr,
	                state->dstaddrlen);
	vPortFree(packet);
	FreeRTOS_closesocket(state->sock);
}

static int prvTftpRrq(struct tftp_client_state *state, const char *name)
{
	size_t namelen = strlen(name) + 1;
	const char *mode = "octet";
	size_t modelen = strlen(mode) + 1;
	static const char options[] = "blksize\0" XSTR(TFTP_MAX_BLKSIZE) "\0"
	                              "windowsize\0" XSTR(TFTP_MAX_WINSIZE);
	size_t optionslen = state->winsize ? 0 : sizeof(options);
	size_t plen = sizeof(struct tftp_xrq) + namelen + modelen + optionslen;

	if (state->retries == TFTP_MAX_RETRIES)
	{
		printf("Failed to receive: RRQ retry limit exceeded\r\n");
		prvTftpTerminate(state, 0, "Retry limit exceeded");
		return 1;
	}
	else
	{
		++state->retries;
	}

	struct tftp_xrq *packet = pvPortMalloc(plen);
	memset(packet, 0, sizeof(*packet));
	packet->header.op = FreeRTOS_htons(TFTP_OP_RRQ);
	char *buf = packet->request;
	memcpy(buf, name, namelen);
	buf += namelen;
	memcpy(buf, mode, modelen);
	buf += modelen;
	memcpy(buf, options, optionslen);

	long err;
	err = FreeRTOS_sendto(state->sock, packet, plen, 0, &state->dstaddr,
	                      state->dstaddrlen);
	vPortFree(packet);
	if (err != (long)plen)
	{
		printf("Failed to send RRQ: FreeRTOS_sendto returned %ld != %ld\r\n",
		       err, (unsigned long)plen);
		return 1;
	}

	return 0;
}

static int prvTftpAck(struct tftp_client_state *state)
{
	uint16_t winlast = state->winstart - 1;
	for (uint16_t bits = state->winbits; bits & 1; bits >>= 1)
		++winlast;

	if (winlast != state->winstart - 1)
	{
		uint16_t numack = winlast - state->winstart + 1;
		state->winbits = 0;
		state->winstart += numack;
		state->winbuf += (numack - 1) * state->blksize;
		if (winlast == state->lastblock)
			state->winbuf += state->lastsize;
		else
			state->winbuf += state->blksize;
		state->retries = 0;
	}
	else if (state->retries == TFTP_MAX_RETRIES)
	{
		printf("Failed to receive: ACK retry limit exceeded\r\n");
		prvTftpTerminate(state, 0, "Retry limit exceeded");
		return 1;
	}
	else
	{
		++state->retries;
	}

	size_t plen = sizeof(struct tftp_ack);
	struct tftp_ack packet;
	memset(&packet, 0, plen);
	packet.header.op = FreeRTOS_htons(TFTP_OP_ACK);
	packet.block = FreeRTOS_htons(winlast);
	long err;
	err = FreeRTOS_sendto(state->sock, &packet, plen, 0, &state->dstaddr,
	                      state->dstaddrlen);
	if (err != (long)plen)
	{
		printf("Failed to send ACK: FreeRTOS_sendto returned %ld != %ld\r\n",
		       err, (unsigned long)plen);
		prvTftpTerminate(state, 0, "Failed to send ACK");
		return 1;
	}

	return 0;
}

static int prvTftpReceive(const char *host, uint16_t port, const char *name, char *buf, size_t *size)
{
	struct tftp_client_state state;
	memset(&state, 0, sizeof(state));

	state.dstaddr.sin_addr = FreeRTOS_inet_addr(host);
	if (!state.dstaddr.sin_addr)
	{
		printf("Invalid IPv4 address: %s\r\n", host);
		return 1;
	}
	state.dstaddr.sin_port = FreeRTOS_htons(port);
	state.dstaddrlen = sizeof(state.dstaddr);

	state.sock = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM,
	                             FREERTOS_IPPROTO_UDP);
	if (state.sock == FREERTOS_INVALID_SOCKET)
	{
		printf("Failed to create socket\r\n");
		return 1;
	}

	BaseType_t timeout = pdMS_TO_TICKS(1000);
	FreeRTOS_setsockopt(state.sock, 0, FREERTOS_SO_RCVTIMEO, (void *)&timeout,
	                    sizeof(BaseType_t));
	FreeRTOS_setsockopt(state.sock, 0, FREERTOS_SO_SNDTIMEO, (void *)&timeout,
	                    sizeof(BaseType_t));

	/* First DATA is block 1. */
	state.winstart = 1;
	if (prvTftpRrq(&state, name))
		return 1;

	uint32_t tstart = 0;
	uint32_t tprev = 0;
	for (;;)
	{
		union
		{
			char buf[sizeof(struct tftp_data) + TFTP_MAX_BLKSIZE + 1];
			struct tftp_header header;
			struct tftp_data data;
			struct tftp_error error;
			struct tftp_oack oack;
		} *upacket;
		struct freertos_sockaddr srcaddr;
		socklen_t srcaddrlen = sizeof(srcaddr);
		long fromlen = FreeRTOS_recvfrom(state.sock, &upacket, 0,
		                                 FREERTOS_ZERO_COPY, &srcaddr,
		                                 &srcaddrlen);
		if (state.winbuf)
		{
			uint32_t tcur = port_get_current_mtime();
			if (tcur - tprev > 5000000)
			{
				size_t scaledbytes = (size_t)(state.winbuf - buf);
				const char *units = "B";
				if (scaledbytes > 1024*1024)
				{
					scaledbytes >>= 20;
					units = "MiB";
				}
				else if (scaledbytes > 1024)
				{
					scaledbytes >>= 10;
					units = "KiB";
				}

				printf("Received %lu %s so far in %us\r\n",
				       (unsigned long)scaledbytes, units,
				       (unsigned int)((tcur - tstart) / 1000000));
				tprev = tcur;
			}
		}
		if (fromlen < 0)
		{
			if (fromlen != -pdFREERTOS_ERRNO_EWOULDBLOCK)
			{
				printf("Failed to receive: FreeRTOS_recvfrom returned: %ld\r\n",
				       fromlen);
				prvTftpTerminate(&state, 0, "Error in FreeRTOS_recvfrom");
				return 1;
			}
			/* Timed out */
			if (!state.pastrrq)
			{
				/* Still trying to initiate a request */
				if (prvTftpRrq(&state, name))
					return 1;
			}
			else
			{
				if (prvTftpAck(&state))
					return 1;
			}
			continue;
		}

		/* Ignore stray packets from another host */
		if (srcaddr.sin_addr != state.dstaddr.sin_addr)
		{
			FreeRTOS_ReleaseUDPPayloadBuffer(upacket);
			continue;
		}

		/*
		 * Ignore stray packets from another port, except the first time, since
		 * that will be our new port/TID for the connection.
		 */
		if (!state.havetid)
		{
			state.havetid = 1;
			state.dstaddr.sin_port = srcaddr.sin_port;
		}
		else if (srcaddr.sin_port != state.dstaddr.sin_port)
		{
			FreeRTOS_ReleaseUDPPayloadBuffer(upacket);
			continue;
		}

		state.recvpacket = upacket;

		if (fromlen < (long)sizeof(upacket->header))
		{
			printf("Failed to receive: packet missing header%ld\r\n", fromlen);
			prvTftpTerminate(&state, 0, "Packet missing header");
			return 1;
		}

		buf[fromlen] = '\0';
		switch (FreeRTOS_ntohs(upacket->header.op))
		{
		case TFTP_OP_ERROR:
			if (fromlen < (long)sizeof(upacket->error))
			{
				printf("Failed to receive: error packet too short (%ld)\r\n",
				       fromlen);
				prvTftpTerminate(&state, 0, "Error packet too short");
				return 1;
			}
			if (!state.winsize)
			{
				/* Old servers might reject RRQ with options; try without */
				state.winsize = 1;
				state.blksize = 512;
				state.retries = 0;
				state.havetid = 0;
				state.pastrrq = 0;
				state.dstaddr.sin_port = FreeRTOS_htons(port);
				if (prvTftpRrq(&state, name))
					return 1;
				break;
			}
			printf("Failed to receive: server code %d: %s\r\n",
			       FreeRTOS_ntohs(upacket->error.code), upacket->error.message);
			return 1;
		default:
			/* Ignore unknown packets for forwards compatibility */
			break;
		case TFTP_OP_OACK:
		{
			/*
			 * Ignore if we've received data packets already; OACK never sent
			 * after DATA
			 */
			if (state.winbuf)
				break;

			if (state.winsize)
			{
				/* Duplicate; retry ACK */
				if (prvTftpAck(&state))
					return 1;
			}

			/* Default values in case server drops our options */
			uint16_t winsize = 1;
			uint16_t blksize = 512;
			/* Parse options from server */
			char *opt = upacket->oack.data;
			while (opt < upacket->buf + fromlen)
			{
				char *val = opt + strlen(opt) + 1;
				if (val >= upacket->buf + fromlen)
				{
					printf("Failed to receive: OACK missing value for option %s\r\n",
					       opt);
					prvTftpTerminate(&state, 0, "Option missing value");
					return 1;
				}
				if (!strcmp(opt, "windowsize"))
				{
					winsize = 0;
					for (int i = 0; val[i]; ++i)
					{
						if (val[i] < '0' || val[i] > '9')
							goto bad_val;
						winsize = winsize * 10 + (val[i] - '0');
						if (winsize > TFTP_MAX_WINSIZE)
							goto bad_val;
					}
					if (winsize == 0)
						goto bad_val;
				}
				else if (!strcmp(opt, "blksize"))
				{
					blksize = 0;
					for (int i = 0; val[i]; ++i)
					{
						if (val[i] < '0' || val[i] > '9')
							goto bad_val;
						blksize = blksize * 10 + (val[i] - '0');
						if (blksize > TFTP_MAX_BLKSIZE)
							goto bad_val;
					}
					if (blksize < 8)
						goto bad_val;
				}
				opt = val + strlen(val) + 1;
				continue;
			bad_val:
				printf("Failed to receive: OACK option %s has bad value %s\r\n",
				       opt, val);
				prvTftpTerminate(&state, 0, "Option has bad value");
				return 1;
			}

			/* OACK is in its own window at the start */
			state.winsize = 1;
			state.winstart = 0;
			state.winbits = 1;
			state.lastblock = 0;
			state.lastsize = 0;
			state.retries = 0;
			if (prvTftpAck(&state))
				return 1;

			state.pastrrq = 1;
			state.winsize = winsize;
			state.blksize = blksize;
			break;
		}
		case TFTP_OP_DATA:
		{
			if (fromlen < (long)sizeof(upacket->data))
			{
				printf("Failed to receive: data packet too short (%ld)\r\n",
				       fromlen);
				prvTftpTerminate(&state, 0, "Data packet too short");
				return 1;
			}

			uint16_t block = FreeRTOS_ntohs(upacket->data.block);
			uint16_t winoff = block - state.winstart;

			if ((winoff < state.winsize || (winoff == 0 && !state.winsize)) &&
			    !state.winbuf)
			{
				state.winbuf = buf;
				state.pastrrq = 1;
				if (!state.winsize)
				{
					/* Server ignored our options and we got no OACK */
					state.winsize = 1;
					state.blksize = 512;
				}
				printf("Transfer started (window size: %u, block size: %d)\r\n",
				       state.winsize, state.blksize);
				tstart = tprev = port_get_current_mtime();
			}

			/*
			 * If we see the last packet from the previous window and have not
			 * yet seen any for this window, re-send the ACK as our previous one
			 * probably got lost.
			 */
			if (winoff == 0xffff && !state.winbits)
			{
				if (prvTftpAck(&state))
					return 1;
			}
			/* Otherwise, ignore packets outside this window */
			if (winoff >= state.winsize)
				break;

			uint16_t blksize = fromlen - sizeof(upacket->data);
			if (blksize > state.blksize)
			{
				printf("Failed to receive: data packet exceeds blksize (%u)\r\n",
				       blksize);
				prvTftpTerminate(&state, 0, "Data packet exceeds blksize");
				return 1;
			}

			memcpy(state.winbuf + winoff * state.blksize,
			       upacket->data.data, blksize);

			uint16_t lastoff = state.lastblock - state.winstart;
			if (winoff > lastoff || lastoff >= state.winsize)
			{
				state.lastblock = block;
				state.lastsize = blksize;
			}

			/* If this is the final packet, truncate this window. */
			if (state.lastsize < state.blksize)
				state.winsize = winoff + 1;

			state.winbits |= 1 << winoff;
			/* Check if we've seen the whole of the window */
			if (state.winbits == (0xffff >> (16 - state.winsize)))
			{
				/* ACK the whole window */
				if (prvTftpAck(&state))
					return 1;

				/* Check if we're done */
				if (state.lastsize < state.blksize)
				{
					FreeRTOS_ReleaseUDPPayloadBuffer(upacket);
					FreeRTOS_closesocket(state.sock);
					*size = state.winbuf - buf;

					uint32_t tcur = port_get_current_mtime();
					size_t scaledbytes = *size;
					const char *units = "B";
					if (scaledbytes > 1024*1024)
					{
						scaledbytes >>= 20;
						units = "MiB";
					}
					else if (scaledbytes > 1024)
					{
						scaledbytes >>= 10;
						units = "KiB";
					}

					printf("Finished receiving %lu %s in %us\r\n",
					       (unsigned long)scaledbytes, units,
					       (unsigned int)((tcur - tstart) / 1000000));
					return 0;
				}
			}
		}
		}

		FreeRTOS_ReleaseUDPPayloadBuffer(state.recvpacket);
		state.recvpacket = NULL;
	}
}
