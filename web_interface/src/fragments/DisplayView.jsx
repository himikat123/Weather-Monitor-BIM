import React, { useRef, useEffect, useState } from 'react';
import Symb_home from '../img/symb/home';
import Symb_hum from '../img/symb/hum';
import Symb_pres from '../img/symb/pres';
import Symb_temp_minus from '../img/symb/temp-';
import Symb_temp_plus from '../img/symb/temp+';
import Symb_wind from '../img/symb/wind';
import D_0 from '../img/digits/0';
import D_1 from '../img/digits/1';
import D_2 from '../img/digits/2';
import D_3 from '../img/digits/3';
import D_4 from '../img/digits/4';
import D_5 from '../img/digits/5';
import D_6 from '../img/digits/6';
import D_7 from '../img/digits/7';
import D_8 from '../img/digits/8';
import D_9 from '../img/digits/9';
import Icon_big_01_d from '../img/icons/big/01_d';
import Icon_big_01_n from '../img/icons/big/01_n';
import Icon_big_02_d from '../img/icons/big/02_d';
import Icon_big_02_n from '../img/icons/big/02_n';
import Icon_big_04 from '../img/icons/big/04';
import Icon_big_09 from '../img/icons/big/09';
import Icon_big_10 from '../img/icons/big/10';
import Icon_big_11_d from '../img/icons/big/11_d';
import Icon_big_11_n from '../img/icons/big/11_n';
import Icon_big_13 from '../img/icons/big/13';
import Icon_big_50 from '../img/icons/big/50';
import Icon_big_loading from '../img/icons/big/loading';
import Icon_small_01 from '../img/icons/small/01';
import Icon_small_02 from '../img/icons/small/02';
import Icon_small_04 from '../img/icons/small/04';
import Icon_small_09 from '../img/icons/small/09';
import Icon_small_10 from '../img/icons/small/10';
import Icon_small_11 from '../img/icons/small/11';
import Icon_small_13 from '../img/icons/small/13';
import Icon_small_50 from '../img/icons/small/50';
import Icon_small_loading from '../img/icons/small/loading';
import Ant_AP from '../img/ant/ant_AP';
import Ant_1 from '../img/ant/ant_1';
import Ant_2 from '../img/ant/ant_2';
import Ant_3 from '../img/ant/ant_3';
import Ant_4 from '../img/ant/ant_4';
import Bat_1 from '../img/bat/bat_1';
import Bat_2 from '../img/bat/bat_2';
import Bat_3 from '../img/bat/bat_3';
import Bat_4 from '../img/bat/bat_4';
import Symb_upd from "../img/symb/upd";
import Wind_north from "../img/wind/n";
import Wind_north_east from "../img/wind/ne";
import Wind_east from "../img/wind/e";
import Wind_south_east from "../img/wind/se";
import Wind_south from "../img/wind/s";
import Wind_south_west from "../img/wind/sw";
import Wind_west from "../img/wind/w";
import Wind_north_west from "../img/wind/nw";

export default function DisplayView(props) {  
    const canvasRef = useRef(null);
    const BG_COLOR          = '#000';
    const FRAME_COLOR       = '#00F';
    const TEXT_COLOR        = '#FFF';
    const TEMPERATURE_COLOR = '#FF0';
    const TEMP_MIN_COLOR    = '#F80';
    const HUMIDITY_COLOR    = '#0FF';
    const PRESSURE_COLOR    = '#F0F';
    const CLOCK_COLOR       = '#0F0';
    const BATTERY_COLOR     = '#0F0';

    const LEFT = 'left';
    const CENTER = 'center';
    const RIGHT = 'right';

    const FONT1 = 14;
    const FONT2 = 21;
    const FONT3 = 29;

    const [clockPointsState, setClockPointsState] = useState(false);
    const [prevTime, setPrevTime] = useState(-1);
    const [prevWeekday, setPrevWeekday] = useState("");
    const [prevComfort, setPrevComfort] = useState(0); 
    const [prevTempIn, setPrevTempIn] = useState(-40400);
    const [prevTempOut, setPrevTempOut] = useState(-40400);
    const [prevHumIn, setPrevHumIn] = useState(-40400);
    const [prevHumOut, setPrevHumOut] = useState(-40400);
    const [prevPresOut, setPrevPresOut] = useState(-40400);
    const [prevIcon, setPrevIcon] = useState(404);
    const [prevDescr, setPrevDescr] = useState("");
    const [prevWindSpeed, setPrevWindSpeed] = useState(-404);
    const [prevWindDirection, setPrevWindDirection] = useState(-1);
    const [prevUpdTime, setPrevUpdTime] = useState(0);
    const [prevDailyWeekday, setPrevDailyWeekday] = useState(["", "", ""]);
    const [prevDailyIcon, setPrevDailyIcon] = useState([404, 404, 404]);
    const [prevTempDailyMax, setPrevTempDailyMax] = useState([-40400, -40400, -40400]);
    const [prevTempDailyMin, setPrevTempDailyMin] = useState([-40400, -40400, -40400]);
    const [prevWindSpeedDaily, setPrevWindSpeedDaily] = useState([-1, -1, -1]);
    const [prevAnt, setPrevAnt] = useState(0);
    const [prevBatLevel, setPrevBatLevel] = useState(10);
    const [prevVolt, setPrevVolt] = useState(-40400.00);

    function drawLine(ctx, x, y, w, h, color) {
        ctx.strokeStyle = color;
        ctx.beginPath();
        ctx.moveTo(x, y);
        ctx.lineTo(x + w, y + h);
        ctx.stroke();
    }

    function fillCircle(ctx, x, y, r, color) {
        ctx.beginPath();
        ctx.arc(x, y, r, 0, 2 * Math.PI);
        ctx.fillStyle = color;
        ctx.fill();
    }

    function drawRoundRect(ctx, x, y, w, h, r, color) {
        ctx.strokeStyle = color;
        ctx.beginPath();
        ctx.roundRect(x, y, w, h, r);
        ctx.stroke();
    }

    function fillRect(ctx, x, y, w, h, color) {
        ctx.fillStyle = color;
        ctx.fillRect(x, y, w, h);
    }

    function drawImage(ctx, image, x, y) {
        let img = new Image();
        img.onload = () => ctx.drawImage(img, x, y);
        img.src = image;
    }

    function printText(ctx, x, y, w, h, text, font, align, color) {
        fillRect(ctx, x, y, w, h, BG_COLOR);
        ctx.font = `${font}px Ubuntu`;
        ctx.fillStyle = color;
        ctx.textAlign = align;
        if(align == CENTER) x += w / 2;
        if(align == RIGHT) x += w;
        ctx.fillText(text, x, y + font - font / 4);
    }

    function showDigit(ctx, dig, x) {
        switch(dig) {
            case 0: drawImage(ctx, D_0(), x, 0); break;
            case 1: drawImage(ctx, D_1(), x, 0); break;
            case 2: drawImage(ctx, D_2(), x, 0); break;
            case 3: drawImage(ctx, D_3(), x, 0); break;
            case 4: drawImage(ctx, D_4(), x, 0); break;
            case 5: drawImage(ctx, D_5(), x, 0); break;
            case 6: drawImage(ctx, D_6(), x, 0); break;
            case 7: drawImage(ctx, D_7(), x, 0); break;
            case 8: drawImage(ctx, D_8(), x, 0); break;
            case 9: drawImage(ctx, D_9(), x, 0); break;
            default: fillRect(ctx, x, 0, 32, 78, BG_COLOR); break;
        }
    }

    function showTime(ctx) {
        let hr = props.data.time.split(':')[0];
        let mn = props.data.time.split(':')[1];
        let currTime = hr * 100 + mn;
        if(prevTime != currTime) {
            if(hr < 10) showDigit(ctx, 10, 0);
            else showDigit(ctx, Math.floor(hr / 10), 0);
            showDigit(ctx, hr % 10, 33);
            showDigit(ctx, Math.floor(mn / 10), 77);
            showDigit(ctx, mn % 10, 109);
            setPrevTime(currTime);
        }
    }

    function showWeekday(ctx, wd) {
        if(prevWeekday != wd) {
            printText(ctx, 146, 6, 40, 20, wd, FONT2, LEFT, CLOCK_COLOR);
            setPrevWeekday(wd);
        }
    }

    function clockPoints(ctx) {
        fillCircle(ctx, 71, 24, 3, clockPointsState ? CLOCK_COLOR : BG_COLOR);
        fillCircle(ctx, 71, 52, 3, clockPointsState ? CLOCK_COLOR : BG_COLOR);
    }

    function showAntenna(ctx) {
        let rssi = parseInt(props.data.network.sig, 10);
        if(prevAnt != rssi) {
            if(rssi > -51) drawImage(ctx, Ant_4(), 292, 1);
            if(rssi < -50 && rssi > -76) drawImage(ctx, Ant_3(), 292, 1);
            if(rssi <- 75 && rssi > -96) drawImage(ctx, Ant_2(), 292, 1);
            if(rssi < -95) drawImage(ctx, Ant_1(), 292, 1);
            if(rssi >= 0) drawImage(ctx, Ant_AP(), 292, 1);
            setPrevAnt(rssi);
        }
    }

    function showTemperature(ctx, temp, x, y, font, color) {
        printText(ctx, x, y, font == FONT3 ? 70 : 56, font == FONT3 ? 26 : 20, (temp >= -50 && temp < 100) ? `${temp}°C` : '--°C', font, font == FONT3 ? CENTER : RIGHT, color);
    }

    function showTemperatureInside(ctx, temp) {
        if(prevTempIn != temp) {
            showTemperature(ctx, temp, 173, 53, FONT3, TEMPERATURE_COLOR);
            setPrevTempIn(temp);
        }
    }

    function showTemperatureOutside(ctx, temp) {
        if(prevTempOut != temp) {
            if(temp < 0) drawImage(ctx, Symb_temp_minus(), 62, 104);
            else drawImage(ctx, Symb_temp_plus(), 62, 104);
            showTemperature(ctx, temp, 71, 113, FONT3, TEMPERATURE_COLOR);
            setPrevTempOut(temp);
        }
    }

    function showHumidity(ctx, hum, x, y) {
        printText(ctx, x, y, 58, 20, (hum >= 0 && hum <= 100) ? `${hum}%` : '--%', FONT2, CENTER, HUMIDITY_COLOR);
    }

    function showHumidityInside(ctx, hum) {
        if(prevHumIn != hum) {
            showHumidity(ctx, hum, 264, 58);
            setPrevHumIn(hum);
        }
    }

    function showHumidityOutside(ctx, hum) {
        if(prevHumOut != hum) {
            showHumidity(ctx, hum, 164, 119);
            setPrevHumOut(hum);
        }
    }

    function showComfort(ctx) {
        if(props.config.display.source.descr == 1) {
            if(prevComfort != props.data.comfort) {
                printText(ctx, 145, 28, 175, 16, props.data.comfort, FONT1, CENTER, TEXT_COLOR);
                setPrevComfort(props.data.comfort);
            }
        }
    }

    function showBatteryLevel(ctx) {
        let level = 0;
        if(props.config.display.source.bat.sens) {
            // thingspeak
            if(props.config.display.source.bat.sens == 1) {
                if(props.data.thing.expired == -1) {
                    level = props.data.thing.fields[props.config.display.source.bat.thing];
                }
                else level = 0;
            }
        }
        if(prevBatLevel != level) {
            switch(level) {
                case 1: drawImage(ctx, Bat_1(), 258, 2); break;
                case 2: drawImage(ctx, Bat_2(), 258, 2); break;
                case 3: drawImage(ctx, Bat_3(), 258, 2); break;
                case 4: drawImage(ctx, Bat_4(), 258, 2); break;
                default: fillRect(ctx, 258, 2, 32, 21, BG_COLOR);
            }
            setPrevBatLevel(level);
        }
    }

    function showVoltageOrPercentage(ctx) {
        let volt = "--";
        let percent = "--";
  
        if(props.config.display.source.volt.sens == 1) { // Thingspeak
            if(props.data.thing.expired == -1) {
                let value = props.data.thing.fields[props.config.display.source.volt.thing];
                volt = value.toFixed(2);
                percent = String(value);
            }
        
            if(prevVolt != volt) {
                if(props.config.display.source.volt.volt == 0) { // Voltage
                    let v = (!checkVoltage(volt)) ? '--' : volt;
                    v += props.data.units.v;
                    printText(ctx, 198, 10, 58, 16, v, FONT1, RIGHT, BATTERY_COLOR);
                }

                else if(props.config.display.source.volt.volt == 1) { // Percentage
                    let p = (!checkPercentage(percent)) ? '--%' : `${percent}%`;
                    printText(ctx, 198, 10, 58, 16, p, FONT1, RIGHT, BATTERY_COLOR);
                }

                else printText(ctx, 198, 10, 58, 16, " ", FONT1, RIGHT, BATTERY_COLOR);

                setPrevVolt(volt);
            }
        }
        else printText(ctx, 198, 10, 58, 16, " ", FONT1, RIGHT, BATTERY_COLOR);
    }
    
    function showWeatherIcon(ctx) {
        let icon = props.data.weather.icon, isDay = props.data.weather.isDay;
        if(prevIcon != (icon * 100 + isDay)) {
            switch(icon) {
                case 1: drawImage(ctx, isDay ? Icon_big_01_d() : Icon_big_01_n(), 0, 104); break;
                case 2: drawImage(ctx, isDay ? Icon_big_02_d() : Icon_big_02_n(), 0, 104); break;
                case 3: drawImage(ctx, isDay ? Icon_big_02_d() : Icon_big_02_n(), 0, 104); break;
                case 4: drawImage(ctx, Icon_big_04(), 0, 104); break;
                case 9: drawImage(ctx, Icon_big_09(), 0, 104); break;
                case 10: drawImage(ctx, Icon_big_10(), 0, 104); break;
                case 11: drawImage(ctx, isDay ? Icon_big_11_d() : Icon_big_11_n(), 0, 104); break;
                case 13: drawImage(ctx, Icon_big_13(), 0, 104); break;
                case 50: drawImage(ctx, Icon_big_50(), 0, 104); break;
                default: drawImage(ctx, Icon_big_loading, 0, 104); break;
            }
            setPrevIcon(icon * 100 + isDay);
        }
    }

    function showDescription(ctx) {
        let descr = props.data.weather.descript;
        if(prevDescr != descr) {
            ctx.font = '21px Ubuntu';
            let w = ctx.measureText(descr).width;
            printText(ctx, 0, 84, 319, 20, descr, w > 316 ? FONT1 : FONT2, CENTER, TEXT_COLOR);
            setPrevDescr(descr);
        }
    }

    function showPressure(ctx) {
        let pres = getPres();
        if(prevPresOut != pres) {
            let p = (pres >= 800 && pres <= 1200) ? Math.round(pres * 0.75) : '--';
            p += props.data.units.mm;
            printText(ctx, 250, 119, 70, 20, p, FONT2, CENTER, PRESSURE_COLOR);
            setPrevPresOut(pres);
        }
    }

    function showWindSpeed(ctx) {
        let speed = props.data.weather.wind.speed;
        if(prevWindSpeed != speed) {
            let w = (speed >= 0 && speed < 100) ? Math.round(speed) : '--';
            w += props.data.units.ms;
            printText(ctx, 95, 146, 40, 16, w, FONT1, LEFT, TEXT_COLOR);
            setPrevWindSpeed(speed);
        }
    }

    function showWindDirection(ctx) {
        let windDirection = props.data.weather.wind.dir;
        if(prevWindDirection != windDirection) {
            if((windDirection >= 338 && windDirection < 360) || 
                (windDirection >= 0 && windDirection < 22)
            ) drawImage(ctx, Wind_north(), 133, 143);
            else if(windDirection >= 22 && windDirection < 67) drawImage(ctx, Wind_north_east(), 133, 143);
            else if(windDirection >= 67 && windDirection < 112) drawImage(ctx, Wind_east(), 133, 143);
            else if(windDirection >= 112 && windDirection < 157) drawImage(ctx, Wind_south_east(), 133, 143);
            else if(windDirection >= 157 && windDirection < 202) drawImage(ctx, Wind_south(), 133, 143);
            else if(windDirection >= 202 && windDirection < 247) drawImage(ctx, Wind_south_west(), 133, 143);
            else if(windDirection >= 247 && windDirection < 292) drawImage(ctx, Wind_west(), 133, 143);
            else if(windDirection >= 292 && windDirection < 338) drawImage(ctx, Wind_north_west(), 133, 143);
            else fillRect(ctx, 133, 143, 16, 16, BG_COLOR);
            setPrevWindDirection(windDirection);
        }
    }

    function showUpdTime(ctx) {
        if(prevUpdTime != props.data.weather.upd) {
            printText(ctx, 184, 148, 133, 16, props.data.weather.upd, FONT1, RIGHT, TEXT_COLOR);
            let w = ctx.measureText(props.data.weather.upd).width;
            if(w) drawImage(ctx, Symb_upd(), 300 - w, 147);
            setPrevUpdTime(props.data.weather.upd);
        }
    }

    function showForecast(ctx, x, num) {
        let tempMax = props.data.weather.daily[num].tMax; 
        let tempMin = props.data.weather.daily[num].tMin; 
        let wind = props.data.weather.daily[num].wind; 

        if(prevDailyIcon[num] != props.data.weather.daily[num].icon) {
            switch(props.data.weather.daily[num].icon) {
                case 1: drawImage(ctx, Icon_small_01(), x + 2, 183); break;
                case 2: drawImage(ctx, Icon_small_02(), x + 2, 183); break;
                case 3: drawImage(ctx, Icon_small_02(), x + 2, 183); break;
                case 4: drawImage(ctx, Icon_small_04(), x + 2, 183); break;
                case 9: drawImage(ctx, Icon_small_09(), x + 2, 183); break;
                case 10: drawImage(ctx, Icon_small_10(), x + 2, 183); break;
                case 11: drawImage(ctx, Icon_small_11(), x + 2, 183); break;
                case 13: drawImage(ctx, Icon_small_13(), x + 2, 183); break;
                case 50: drawImage(ctx, Icon_small_50(), x + 2, 183); break;
                default: drawImage(ctx, Icon_small_loading(), x + 2, 183); break;
            }
            let dailyIcon = prevDailyIcon;
            dailyIcon[num] = props.data.weather.daily[num].icon;
            setPrevDailyIcon(dailyIcon);
        }

        if(prevDailyWeekday[num] != props.data.wd[num + 1]) {
            printText(ctx, x + 33, 168, 40, 16, props.data.wd[num + 1], FONT1, CENTER, TEXT_COLOR);
            let dailyWeekday = prevDailyWeekday;
            dailyWeekday[num] = props.data.wd[num + 1];
            setPrevDailyWeekday(dailyWeekday);
        }

        if(prevTempDailyMax[num] != Math.round(tempMax)) {
            showTemperature(ctx, Math.round(tempMax), x + 49, 183, FONT2, TEMPERATURE_COLOR);
            let tempDailyMax = prevTempDailyMax;
            tempDailyMax[num] = Math.round(tempMax);
            setPrevTempDailyMax(tempDailyMax);
        }

        if(prevTempDailyMin[num] != Math.round(tempMin)) {
            showTemperature(ctx, Math.round(tempMin), x + 49, 203, FONT2, TEMP_MIN_COLOR);
            let tempDailyMin = prevTempDailyMin;
            tempDailyMin[num] = Math.round(tempMin);
            setPrevTempDailyMin(tempDailyMin);
        }
        
        if(prevWindSpeedDaily[num] != Math.round(wind)) {
            let w = (wind >= 0 && wind < 100) ? Math.round(wind) : '--';
            w += props.data.units.ms;
            printText(ctx, x + 31, 224, 44, 15, w, FONT1, CENTER, TEXT_COLOR);
            let windSpeedDaily = prevWindSpeedDaily;
            windSpeedDaily[num] = Math.round(wind);
            setPrevWindSpeedDaily(windSpeedDaily);
        }
    }

    function getTemp(sens, thing) {
        let temp = 40400.0;
        switch(sens) {
            case 1: temp = props.data.weather.temp; break;
            case 2: if(props.data.thing.expired == -1) temp = props.data.thing.fields[thing]; break;
            case 3: temp = props.data.bme280.temp + props.config.sensors.bme280.t; break;
            case 4: temp = props.data.bmp180.temp + props.config.sensors.bmp180.t; break;
            case 5: temp = props.data.sht21.temp + props.config.sensors.sht21.t; break;
            case 6: temp = props.data.dht22.temp + props.config.sensors.dht22.t; break;
            case 7: temp = props.data.ds18b20.temp + props.config.sensors.ds18b20.t; break;
            default: ; break;
        }
        return Math.round(temp);
    }

    function getHum(sens, thing) {
        let hum = 40400.0;
        switch(sens) {
            case 1: hum = props.data.weather.hum; break;
            case 2: if(props.data.thing.expired == -1) hum = props.data.thing.fields[thing]; break;
            case 3: hum = props.data.bme280.hum + props.config.sensors.bme280.h; break;
            case 4: hum = props.data.sht21.hum + props.config.sensors.sht21.h; break;
            case 5: hum = props.data.dht22.hum + props.config.sensors.dht22.h; break;
            default: ; break;
        }
        return Math.round(hum);
    }

    function getPres() {
        let pres = 40400.0;
        switch(props.config.display.source.presOut.sens) {
            case 1: pres = props.data.weather.pres; break;
            case 2: if(props.data.thing.expired == -1) pres = props.data.thingspeak[props.config.display.source.presOut.thing]; break;
            case 3: pres = props.data.sensors.bme280.p + props.config.sensors.bme280.p; break;
            case 4: pres = props.data.sensors.bmp180.p + props.config.sensors.bmp180.p; break;
            default: ; break;
        }
        return pres;
    }

    function checkVoltage(val) {
        if(val >= 0.0 && val <= 12.0) return true;
        else return false;
    }

    function checkPercentage(val) {
        if(val >= 0.0 && val <= 100.0) return true;
        else return false;
    }

    function drawSkeleton(ctx) {
        fillRect(ctx, 0, 0, ctx.canvas.width, ctx.canvas.height, BG_COLOR);
        drawLine(ctx, 0, 80, 319, 0, FRAME_COLOR);
        drawLine(ctx, 143, 2, 0, 75, FRAME_COLOR);
        drawRoundRect(ctx, 0, 165, 106, 75, 10, FRAME_COLOR);
        drawRoundRect(ctx, 106, 165, 106, 75, 10, FRAME_COLOR);
        drawRoundRect(ctx, 212, 165, 107, 75, 10, FRAME_COLOR);
        drawImage(ctx, Symb_home(), 145, 48);
        drawImage(ctx, Symb_hum(), 243, 48);
        drawImage(ctx, Symb_temp_plus(), 62, 104);
        drawImage(ctx, Symb_hum(), 143, 109);
        drawImage(ctx, Symb_pres(), 222, 109);
        drawImage(ctx, Symb_wind(), 61, 146);
    }

    const draw = (ctx) => {
        showTime(ctx);
        clockPoints(ctx);
        showWeekday(ctx, props.data.wd[1]);
        showBatteryLevel(ctx);
        showVoltageOrPercentage(ctx);
        showAntenna(ctx);

        showComfort(ctx);
        showTemperatureInside(ctx, getTemp(props.config.display.source.tempIn.sens, props.config.display.source.tempIn.thing));
        showHumidityInside(ctx, getHum(props.config.display.source.humIn.sens, props.config.display.source.humIn.thing));

        showWeatherIcon(ctx);
        showDescription(ctx);
        showTemperatureOutside(ctx, getTemp(props.config.display.source.tempOut.sens, props.config.display.source.tempOut.thing));
        showHumidityOutside(ctx, getHum(props.config.display.source.humOut.sens, props.config.display.source.humOut.thing));
        showPressure(ctx);
        showWindSpeed(ctx);
        showWindDirection(ctx);
        showUpdTime(ctx);
        
        showForecast(ctx, 0, 0);
        showForecast(ctx, 106, 1);
        showForecast(ctx, 212, 2);
    }

    useEffect(() => {
        const canvas = canvasRef.current;
        const context = canvas.getContext('2d');
        drawSkeleton(context);
    }, []);
  
    useEffect(() => {
        const canvas = canvasRef.current;
        const context = canvas.getContext('2d');

        const render = () => {
            draw(context);
        }

        const int = setInterval(() => {
            render();
            setClockPointsState((clockPointsState) => !clockPointsState);
        }, 500);
        return () => clearInterval(int);

    }, [draw, clockPointsState]);

    return <canvas width="320" height="240" ref={canvasRef} style={{margin: 0, padding: 0, width: '320px', height: '240px'}}/>
}