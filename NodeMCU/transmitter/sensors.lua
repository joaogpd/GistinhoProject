require "setupwifi"

local dhtPin = 2

function startADC() 
    if adc.force_init_mode(adc.INIT_ADC) then
      node.restart()
      return 
    end
end

function startTimer(time, callback)
    tmr.create():alarm(time, tmr.ALARM_AUTO, callback)
end

function convertAnalogToTemperature(voltage)
    return ((voltage / 1024.0) * 3000) / 10;
end

function readTemperature()
    local temp = math.floor(convertAnalogToTemperature(adc.read(0)))
    print("Temperature: " .. temp)
    return temp
end

function readHumidity() 
    status, temperature, humidity, temp_dec, humi_dec = dht.read(dhtPin)
    if status == dht.OK then 
        print("Humidity: " .. 100 - humidity)
        return 100 - humidity
    else
        -- TODO: trocar para retorno de constante
        return "X"
    end
end

function printValue(text, value1, value2)
    if value1 and value2 then
        print(text .. value1 .. " " .. value2)
    elseif value1 then
        print(text .. value1)
    elseif value2 then
        print(text .. value2)
    end
end

function readSensors(temp, humi) 
    local temp = temp or true
    local humi = humi or true
    if temp then
        printValue("Temperatura: ", readTemperature())
    end
    if humi then
        printValue("Umidade: ", readHumidity())
    end
end
