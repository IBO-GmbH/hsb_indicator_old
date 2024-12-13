minicom: https://www.acmesystems.it/minicom

module: sc16is7xx docs: https://elixir.bootlin.com/linux/v4.1/source/Documentation/devicetree/bindings/serial/nxp,sc16is7xx.txt
chip: sc16is740

configure serial
```bash
# set it to raw
stty -F /dev/ttySC0 115200 raw -echo -echoe -echok
# check values
stty -F /dev/ttySC0 -a
```

device tree tutorial: https://www.raspberrypi.org/documentation/configuration/device-tree.md

sample device tree config https://www.raspberrypi.org/forums/viewtopic.php?t=217360

more links with device tree generation:
- https://www.raspberrypi.org/forums/viewtopic.php?t=128892
- link to rpi-kernel mr https://github.com/raspberrypi/linux/issues/1594

sc16is740-overlay.dts
```dts
/dts-v1/;
/plugin/;
/ {
        compatible = "brcm,bcm2835", "brcm,bcm2708", "brcm,bcm2709";
        fragment@0 {
                target = <&i2c_arm>;
                __overlay__ {
                        #address-cells = <1>;
                        #size-cells = <0>;
                        status = "okay";
                        sc16is740: sc16is740@48 {
                                compatible = "nxp,sc16is740";
                                reg = <0x48>; /* address */
                                clocks = <&sc16is740_clk>;
                                interrupt-parent = <&gpio>;
                                interrupts = <19 2>; /* IRQ_TYPE_EDGE_FALLING */
                                sc16is740_clk: sc16is740_clk {
                                        compatible = "fixed-clock";
                                        #clock-cells = <0>;
                                        clock-frequency = <14745600>; /* XTAL freq */
                                };
                        };
                };
        };
        
        fragment@1 {
                target = <&gpio>;
                __overlay__ {
                        sc16is740_pins: sc16is740_pins {
                                brcm,pins = <19>;
                                brcm,function = <0>; /* in */
                        };
                };
        };
        
        __overrides__ {
                addr = <&sc16is740>,"reg:0";
                clkrate = <&sc16is740_clk>,"clock-frequency:0";
                irqpin = <&sc16is740>,"interrupts:0",<&sc16is740_pins>,"brcm,pins:0";
        };
};
```

build overlay file
```bash
#!/bin/bash

set -e
cd "$(dirname "$0")"
dtc -@ -I dts -O dtb -o sc16is740-overlay.dtb sc16is740-overlay.dts
cp -v sc16is740-overlay.dtb /boot/overlays
```

`/boot/config.txt`
```
dtoverlay=sc16is740,clkrate=1843200,irqpin=19
```
