require "sensors"
require "setupwifi"

local function start()
    startADC()
    setupWiFi()
end

tmr.create():alarm(10000, tmr.ALARM_SINGLE, start)
