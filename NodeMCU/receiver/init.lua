require "setupwifi"

local function start()
    setupI2C()
    setupWiFi()
end

tmr.create():alarm(10000, tmr.ALARM_SINGLE, start)
