local bus_id = 0
local arduino_address = 0x20
local sda_pin = 2
local scl_pin = 1
local srv
local msg_temp = 0
local msg_humi = 0
local temp_or_humi = 0

function sendPlaySong() 
    sendI2C(1)
end

local actions = {
    MUSICA = sendPlaySong
}

function setupI2C() 
    i2c.setup(bus_id, sda_pin, scl_pin, i2c.FAST)
    print("i2c was setup")
end

function sendI2C(message)
    print("Started sending through i2c")
    i2c.start(bus_id)
    i2c.address(bus_id, arduino_address, i2c.TRANSMITTER)
    i2c.write(bus_id, tonumber(message))
    i2c.stop(bus_id)
end

function doConnection() 
    local sensorClient = mqtt.Client("receiver", 120)

    function subscribeTopics()
        sensorClient:subscribe({
            ["sensordata/temperature"] = 0,
            ["sensordata/humidity"] = 0}, 
            function(c) 
                function send(client, topic, message)
                    if temp_or_humi == 0 then
                        msg_temp = message
                        temp_or_humi = 1
                    elseif temp_or_humi == 1 then
                        msg_humi = message
                        temp_or_humi = 0
                    end
                    sendI2C(message)
                    print("Got: " .. message .. " from topic " .. topic .. " and sent it through i2c")
                end
                print("Subscribed to temperature and humidity")
                c:on("message", send)
            end)
   end

   sensorClient:on("connect", subscribeTopics)

   local MQTTport = 0
   sensorClient:connect("MQTTip", MQTTport, false, conectado, 
        function(client, reason) print("failed reason: " .. reason) end)
   print("connected to MQTT")
end

local function receiver(sck, request)
    if not request then 
        sck:send("<h1>Panic! At The Disco</h1>",
            function() 
                print("Recebeu")
                sck:close() 
            end)
        return 
    end
    
    local _, _, method, path, vars = string.find(request, "([A-Z]+) ([^?]+)%?([^ ]+) HTTP")
    print("vars: " .. (vars or "nada"))
    
    if method == nil then
        _, _, method, path = string.find(request, "([A-Z]+) (.+) HTTP")
    end
    
    local _GET = {}
    
    if vars ~= nil then
        for k, v in string.gmatch(vars, "(%w+)=(%w+)&*") do
            _GET[k] = v
        end
    end
    

    local action = actions[_GET.let]
    if action then action() end
            
    local vals = {
        TEMP = msg_temp,
        HUMI = msg_humi
    }

    
    local response = {}
    response[#response + 1] = [[
    <!DOCTYPE html>
        <head> 
            <meta charset="utf-8" />
            <meta name="viewport" content="width=device-width, initial-scale=1">
            
            <title>
                Gistinho
            </title>
            <style>
                * {
                  box-sizing: border-box;
                }
                body {
                  font-family: Tahoma, Verdana, sans-serif;
                  margin: 0;
                  background-color: #F0BDFF;
                }
                .header {
                  background-color: #f1f1f1;
                  padding: 20px;
                  text-align: center;
                  background-color: #702AA7;
                  font-size: 30px;
                  color: #FFFFFF;
                }
    ]]
    response[#response + 1] = [[
                .column {
                  float: center;
                  width: 100%;
                  padding: 20px;
                  text-align: center;
                }
                .button {
                  background-color: #3A0079;
                  border: none;
                  color: white;
                  padding: 15px 32px;
                  text-align: center;
                  text-decoration: none;
                  display: inline-block;
                  font-size: 18px;
                  margin: 4px 2px;
                  cursor: pointer;
                  border-radius: 12px;
                }
                .button:hover {
                  background-color: #6C00E2;
                  color: white;
                }
                .info {
                 
                }
                @media screen and (max-width:600px) {
                  .column {
                    width: 100%;
                  }
                }
            </style>
        </head>
    ]]
    response[#response + 1] = string.gsub([[
        <body>
            <div class="header">
                <h1>GISTINHO</h1>
            </div>
            <div class="row">
                <div class="column">
                    <h2>
                        <div class="info">TEMPERATURA: </div>
                        <div>$TEMP &#176C </div>
                    </h2>
                    <h2>
                        <div class="info">UMIDADE: </div>
                        <div>$HUMI &#37 </div>
                    </h2>
                    </br>
                    <a href="?let=MUSICA"><button class="button">OI! \o/</button></a>
                </div>
            </div>
        </body>
    </html>
    ]], "$(%w+)", vals)
    
    local function send(lsck)
        if #response > 0 then
            lsck:send(table.remove(response, 1))
        else
            lsck:close()
            response = nil
        end
    end

    sck:on("sent", send)
        
    send(sck)
end

function setupTCP()
    srv = net.createServer(net.TCP)

    if srv then
        srv:listen(80,
            function(conn)
                conn:on("receive", receiver)
            end)  
    end
end

function setupWiFi() 
    local st_cfg = {
      ssid = "wifissid",
      pwd = "wifipassword",
      save = false,
      got_ip_cb = function(con)
                    print(con.IP)
                    setupTCP()
                    doConnection()
                  end
    }

    local ap_cfg = {
        ssid  = "Gistinho", 
        pwd = "gistinho", 
        save = false,
        staconnected_cb = function(cli) 
            print("Alguém conectou! MAC: " .. cli.MAC) 
        end,
        stadisconnected_cb = function(cli) 
            print("Alguém desconectou! MAC: " .. cli.MAC) 
        end
    }

    print("modo: " .. wifi.setmode(wifi.STATIONAP))
    print(wifi.sta.config(st_cfg))
    wifi.ap.config(ap_cfg)
    wifi.ap.setip({ip = "192.168.0.20",
        netmask = "255.255.255.0", 
        gateway = "192.168.0.20"})
    print(wifi.ap.getip())
end
