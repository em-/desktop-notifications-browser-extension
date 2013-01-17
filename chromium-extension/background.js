var plugin = document.getElementById("desktop-notifications-plugin");
console.log("dn: Desktop Notification background script loading");
chrome.extension.onMessage.addListener(function(request, sender, sendResponse) {
    console.log("dn: Message received", request);
    if (typeof(request) != 'object' || request['type'] != 'show-notification')
        return;
    console.log("dn: Show native notification", request.title, request.body, request.icon);
    plugin.showNotification(request.title, request.body, request.icon);
});
