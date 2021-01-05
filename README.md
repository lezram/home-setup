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
  * Add pre-config for dhcp within rootfs
    (On macOS you can mount it with ext4fuse; https://piratefache.ch/mount-raspberry-pi-sd-card-on-mac-os/) \
    `/etc/dhcpcd.conf`
    ```
    interface eth0 # or 'wlan0' or any other interface
    static ip_address=192.168.1.2/24
    # static ip6_address=:::::/64
    static routers=192.168.1.1
    static domain_name_servers=8.8.8.8 1.1.1.1 192.168.1.1
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

1. copy the [config](raspberry-pi) to the raspberry pi
2. Create config folders (if config folders are changed, update [`docker-compose.yaml`](raspberry-pi/home-assistant/docker-compose.yaml))
    ```
    mkdir /opt/homeassistant/config
    mkdir /opt/homeassistant/config
    mkdir /opt/mosquitto/config
    mkdir /opt/mosquitto/data
    mkdir /opt/mosquitto/log
    mkdir /opt/bitwarden/data
    mkdir /opt/certs
    ```
4. Run `docker-compose up -d` in folder [`home-assistant`](raspberry-pi/home-assistant)
5. Add mosquitto password file `mosquitto_passwd` (**TODO** add detailed description)
7. Run the python script for raspberry sensors every 5 minutes.
  * **TODO**; detailed `pip install`/dependency instructions
  * Add script to crontab
      ```bash
      crontab -e
      
      */5 * * * * /.../send_data_raspberry_and_dht21.py
      ```

#### ESP8266 + Sensor
Setup inspired by [home-assistant sample](https://www.home-assistant.io/blog/2015/10/11/measure-temperature-with-esp8266-and-report-to-mqtt/).

The 3 ESP8266 have each a BME280 sensor connected
and are configured for MQTT auto-detection with home-assistant.

1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Adapt `esp8366.ino`
3. Write `esp8366.ino` to the controller
3. Plugin the controller


### Bitwarden
1. Go to [bitwarden folder](./raspberry-pi/bitwarden)
2. Run `docker-compose up -d`

## Open
* Detailed mosquitto and home assistant auto detection setup
* python script dependency installation 
* ssl/tls config description
* nginx proxy
