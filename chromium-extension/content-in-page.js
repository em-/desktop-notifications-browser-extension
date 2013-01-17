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
console.log("dn: Desktop Notification in-page content script loading");
// webkitNotifications API
// http://www.chromium.org/developers/design-documents/desktop-notifications/api-specification
if (window.webkitNotifications)
{
    // We're sure to get the real implementation because we run as soon as the DOM is set up
    // (see content-isolated.js)
    var createNotificationReal = webkitNotifications.createNotification.bind(webkitNotifications);
    var checkPermission = webkitNotifications.checkPermission.bind(webkitNotifications);
    console.log("dn: Replacing webkitNotifications.createNotification");
    webkitNotifications.createNotification = function(icon, title, body)
    {
        var PERMISSION_ALLOWED = 0;
        console.log("dn: Creating notification object created");
        var notification = createNotificationReal(icon, title, body);
        if (!notification)
            return notification;
        var showReal = notification.show.bind(notification);
        notification.show = function() {
            if (checkPermission() != PERMISSION_ALLOWED) {
                showReal(); // Let the original method throw DOMException(SECURITY_ERR)
                return; // Not reached
            }
            console.log("dn: Showing native notification");
            // Forward the notification request to the isolated content script, such that it
            // can forward it to the background page with the native plugin. We use this
            // kind of middleman for security reasons: the in-page script is needed to
            // replace createNotification() in the page context but cannot directly
            // communicate with the background page, which is needed because we don't want
            // to expose the plugin to user pages.
            window.postMessage({type: 'show-notification', icon: icon, title: title, body: body}, "*");
        };
        return notification;
    };
}
