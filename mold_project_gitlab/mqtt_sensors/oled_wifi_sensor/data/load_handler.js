function toggleDisable(checkbox) {
    var toggle = document.getElementsByClassName("configField");
    for (let i = 0; i < toggle.length; i++) {
        checkbox.checked ? toggle[i].disabled = false : toggle[i].disabled = true;
    }
}
window.onload = function () {
    var xhr = new XMLHttpRequest;
    xhr.open("GET", ("/settings.txt?" + Math.random()), true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 200) {
            var contents = xhr.responseText;
            var parameters = contents.split(";");

            for (let i = 0; i < (parameters.length - 1); i++) {
                var temp = parameters[i].split("=");
                if (temp[0] == "mqttEnable" || temp[0] == "willRetain" || temp[0] == "proximityEnable")
                    document.getElementById(temp[0]).checked = unescape(temp[1]) == "0" ? false : true;
                else
                    document.getElementById(temp[0]).value = unescape(temp[1]);
            }
            toggleDisable(document.getElementById("mqttEnable"));
        }
    }
    xhr.send();
}
