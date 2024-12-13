# DBus

## links

- https://techbase.kde.org/Development/Tutorials/D-Bus/CustomTypes
- qdbusxml2cpp unknown type https://stackoverflow.com/questions/22253458/qdbusxml2cpp-unknown-type
- types https://www.alteeve.com/w/List_of_DBus_data_types

## generate qt code

```bash
qdbusxml2cpp -p org_desktop_networkmanager -N -i array_of_string_variant.hpp -i connection.hpp -i array_of_string_uint32.hpp -i array_of_string_string.hpp org.freedesktop.NetworkManager.xml

qdbusxml2cpp -p org_desktop_networkmanager_device_wireless -N -i array_of_string_variant.hpp org.freedesktop.NetworkManager.Device.Wireless.xml
```
