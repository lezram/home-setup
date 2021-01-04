# home-setup
My home server setup for home monitoring and non-cloud apps.

* Temperature and humidity monitoring
* [Bitwarden](https://bitwarden.com/) password manager

## Hardware

* Raspberry Pi 4 Model B - 4 GB RAM
* 3 x ESP8266 NodeMCU
* Sensors
  * 1 x DHT21 (Humidity Temperature Sensor)
  * 3 x BME280 (Barometric Sensor)

## Setup

### Raspberry

1. [Write a Raspbian image to the sd-card](https://www.raspberrypi.org/documentation/installation/installing-images/)
  * [Enable SSH on a headless Raspberry Pi](https://www.raspberrypi.org/documentation/remote-access/ssh/)
  * [Add WIFI configuration](https://www.raspberrypi.org/documentation/configuration/wireless/headless.md) \
    Mount sd-card and put `wpa_supplicant.conf` into `boot` folder
    ```
    ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
    update_config=1
    country=US
    
    network={
      ssid="<wifi_ssid>"
      psk="<wifi_password>"
    }
    ```
  * Add dhcp config within rootfs
    (On mac you can mount it with ext4fuse; https://piratefache.ch/mount-raspberry-pi-sd-card-on-mac-os/)
    `/etc/dhcpcd.conf`
    ```
    interface eth0
    static ip_address=192.168.0.5/24
    #static ip6_address=fd51:42f8:caae:d92e::ff/64
    static routers=192.168.0.1
    static domain_name_servers=8.8.8.8 1.1.1.1 192.168.0.1 fd51:42f8:caae:d92e::1
    
    interface wlan0
    static ip_address=192.168.0.6/24
    #static ip6_address=fd51:42f8:caae:d92e::ff/64
    static routers=192.168.0.1
    static domain_name_servers=8.8.8.8 1.1.1.1 192.168.0.1 fd51:42f8:caae:d92e::1
    ```
2. [Install Docker](https://phoenixnap.com/kb/docker-on-raspberry-pi)
    ```
    curl -fsSL https://get.docker.com -o get-docker.sh
    sudo sh get-docker.sh
    sudo usermod -aG docker Pi
    
    sudo apt-get -y install libffi-dev libssl-dev python3-dev python3 python3-pip
    sudo pip3 -v install docker-compose
    ```
3. Insert the sd-card and boot up
4. Login via ssh

### home-assistant

1. copy home-assistant config
2. copy mosquitto
3. Go to home-assistant folder
4. `docker-compose up -d`
5. Add mosquitto mosquitto_passwd
6. Copy custom script `send_data_raspberry_and_dht21.py`
7. Add script to crontab
  ```bash
  crontab -e
  
  */5 * * * * /.../send_data_raspberry_and_dht21.py
  ```

### Bitwarden
1. Go to botwarden folder
2. `docker-compose up -d`

### ESP8266 + Sensor
Setup inspired by [home-assistant sample](https://www.home-assistant.io/blog/2015/10/11/measure-temperature-with-esp8266-and-report-to-mqtt/).

The 3 ESP8266 have each a BME280 sensor connected 
and are configured for MQTT auto-detection with home-assistant.

1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Adapt `esp8366.ino`
3. Write `esp8366.ino` to the controller
3. Plugin the controller


## Open
* SSL configuration needs to be improved
