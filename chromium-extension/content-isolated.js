/* Desktop notifications Chromium extension - content script
 * For info about the webkitNotifications API see
 * http://www.chromium.org/developers/design-documents/desktop-notifications/api-specification
 *
 * Copyright © Collabora Ltd. 2013
 *
 * Author:
 *   Emanuele Aina <emanuele.aina@collabora.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 */

/* Inject the real script in the page, as the isolated context
 * content scripts run in prevent us from replacing the
 * window.webkitNotifications.createNotification() function.
 * Use mutation observers to inject our code as soon as the DOM
 * has been set up, before any other script had a chance to run.
 */
console.log("dn: Desktop Notification content script loading");
var observer = new WebKitMutationObserver(function(mutations, observer) {
    observer.disconnect();
    console.log("dn: Injecting in-page content script");
    var script = document.createElement("script");
    script.src = chrome.extension.getURL("content-in-page.js");
    script.type = "text/javascript";
    document.head.appendChild(script);
});
observer.observe(document, { subtree: true, childList: true });

window.onmessage = function (evt) {
    if (typeof(evt.data) != 'object' || evt.data['type'] != 'show-notification')
        return;
    console.log("dn: Forward notification request to background page", evt.data);
    chrome.extension.sendMessage(evt.data);
}
