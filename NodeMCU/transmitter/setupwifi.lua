publish_timer = tmr.create()

function doConnection() 
    local sensorClient = mqtt.Client("transmitter", 120)

    sensorClient:on("connect", 
        function() 
            publish_timer:start(true)
        end)
    
    function publica()
      sensorClient:publish("sensordata/temperature", readTemperature(), 0, 0, 
                function(client) print("mandou") end)
      sensorClient:publish("sensordata/humidity", readHumidity(), 0, 0,
                function(client) print("mandou") end) 
    end

    publish_timer:register(10000, tmr.ALARM_AUTO, publica)

    local MQTTport = 0
    sensorClient:connect("MQTTip", MQTTport, false, conectado, 
        function(client, reason) print("failed reason: " .. reason) end)
    print("connected to MQTT")
end

function setupWiFi() 
    wificonf = {
      ssid = "wifissid",
      pwd = "wifipassword",
      save = false,
      got_ip_cb = function(con)
                    print(con.IP)
                    doConnection()
                  end
    }
    
    print(wifi.sta.config(wificonf))
    print("modo: " .. wifi.setmode(wifi.STATION))
end
