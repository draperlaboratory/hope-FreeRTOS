// Copyright © 2017-2019 The Charles Stark Draper Laboratory, Inc. and/or Dover Microsystems, Inc.
// All rights reserved.
//
// Use and disclosure subject to the following license.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

function errorSet(text) {
    document.getElementById("error-field").innerHTML = text;
}

function errorClear() {
    errorSet("");
}

function createArgStrPair(field, name) {
    return name + ":" + field + ",";
}

function getXmlFromText(text) {
    var parser = new DOMParser();
    return parser.parseFromString(text,"text/xml")
}

function getXmlValueFromTag(xmlDoc, tag) {
    try {
        return xmlDoc.getElementsByTagName(tag)[0].childNodes[0].nodeValue;
    } catch (err) {
        return "";
    }
}

function getHtmlStringFromTag(xmlDoc, tag) {
    try {
        return xmlDoc.getElementsByTagName(tag)[0].innerHTML;
    } catch (err) {
        return "";
    }
}

function getCookieValue(key) {
  var cookie = document.cookie.match('(^|[^;]+)\\s*' + key + '\\s*=\\s*([^;]+)');
  return cookie ? cookie.pop() : '';
}

function setCookieValue(headerfields) {
  document.cookie = headerfields;
}

function cgiHandler(argStr, script, element, type) {
  var xhttp = new XMLHttpRequest();

  xhttp.onreadystatechange = function() {
    if (this.readyState == 4) {
      var xmlDoc = getXmlFromText(this.responseText);
      var response = {
        exitStatus: parseInt(getXmlValueFromTag(xmlDoc, "exitstatus")),
        headerFields: getXmlValueFromTag(xmlDoc, "headerfields"),
        responseText: getHtmlStringFromTag(xmlDoc, "responseText")
      };

      switch (response.exitStatus) {
        case 200:
          setCookieValue(response.headerFields);

          switch(type) {
            case "load":
              document.getElementById(element).innerHTML = response.responseText;
              break;
            case "redirect":
              window.location.href = response.responseText;
              break;
            default:
              break;
          }
          return response;
        default:
           errorSet(response.exitStatus + ": " + response.responseText);
           return null;
      }
    }
  }

  xhttp.open("POST", "../cgi-bin/" + script, true);
  xhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded; charset=UTF-8");
  xhttp.send("userInput=ARGS" + argStr + "ENDARGS");
}

function loadCgiContent(argStr, script, element) {
  cgiHandler(argStr, script, element, "load");
}

function submitCgiForm(argStr, script) {
  cgiHandler(argStr, script, "", "redirect");
}
