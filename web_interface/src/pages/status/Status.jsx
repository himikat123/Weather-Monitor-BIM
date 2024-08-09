import StatusFn from "./StatusFn";
import Languages from "../../Languages";
import PageHeader from "../../fragments/PageHeader";
import DisplayView from "../../fragments/DisplayView";
import {ReactComponent as ArrowImage} from "../../svg/arrow.svg";
import Menu from "../../menu/Menu";

class Status extends StatusFn {
    render() {
        let text = new Languages();
        let lang = this.props.config.lang;
        let url = "https://github.com/himikat123/Weather-Monitor-BIM";

        return (<>
            <Menu language={lang} />
            
            <div className="d-flex justify-content-center">
                <div className="content m-2 m-lg-5">
                    <PageHeader h="2" text={text.get('status', lang)} />

                    {this.props.newVersion && 
                        <div className="text-center border border-secondary rounded m-3 p-3">
                            {text.get('firmwareUpdateAppeared', lang).replace(new RegExp("XXX", "g"), this.props.gitVersion)}<br />
                            <a href={url}>{url}</a>
                        </div>
                    }

                    {this.props.data.hasOwnProperty('network') && <>
                    <div className="row">
                        {/* Display */}
                        <div className="col text-center state border border-secondary rounded m-4 p-3 row justify-content-center status-tile d-flex align-items-center">
                            <div className="border border-5 border-secondary rounded m-0 p-0 w-auto" style={{height: '249px'}}>
                                <DisplayView config={this.props.config} data={this.props.data} />
                            </div>
                        </div>
                        
                        {/* System  */}
                        <div className="col text-center state border border-secondary rounded m-4 p-3 row d-flex flex-column align-items-start status-tile">
                            <div className="fs-4 mb-3">{text.get('system', lang)}</div>
                            <div className="mt-0">{text.get('firmwareVersion', lang)}</div>
                            <div className="indication">{this.props.data.fw}</div>
                            <div className="mt-1">{text.get('runtime', lang)}</div>
                            <div className="indication">{this.props.data.runtime}</div>
                            <div className="mt-1">{text.get('systemTimeAndDate', lang)}</div>
                            <div className="indication">
                                {this.props.data.time.split(' ')[0]}<br />
                                {this.props.data.time.split(' ')[1]}
                            </div>
                        </div>
                    </div>

                    <div className="row">
                        {/* Network */}
                        <div className="col text-center state border border-secondary rounded m-4 p-3 row status-tile">
                            <div className="fs-4 mb-3">{text.get('network', lang)}</div>
                            <div className="mt-0">{text.get('networkName', lang)}</div>
                            <div className="indication">{this.props.data.network.ssid}</div>
                            <div className="mt-1">{text.get('channelNumber', lang)}</div>
                            <div className="indication">{this.props.data.network.ch}</div>
                            <div className="mt-1">{text.get('signalStrength', lang)}</div>
                            <div className="indication">{this.props.data.network.sig}</div>
                            <div className="mt-1">{text.get('macAddress', lang)}</div>
                            <div className="indication">{this.props.data.network.mac}</div>
                            <div className="mt-1">{text.get('ipAddress', lang)}</div>
                            <div className="indication">{this.props.data.network.ip}</div>
                            <div className="mt-1">{text.get('subnetMask', lang)}</div>
                            <div className="indication">{this.props.data.network.mask}</div>
                            <div className="mt-1">{text.get('defaultGateway', lang)}</div>
                            <div className="indication">{this.props.data.network.gw}</div>
                        </div>

                        {/* Sensors */}
                        <div className="col text-center state border border-secondary rounded m-4 p-3 row status-tile">
                            <div className="fs-4 mb-3">{text.get('sensors', lang)}</div>
                            <div className="mt-0">BME280</div>
                            <div className="indication">{this.props.config.hasOwnProperty('sensors') && (
                                this.temp(this.props.data.bme280.temp, this.props.config.sensors.bme280.t) + '°C, ' +
                                this.hum(this.props.data.bme280.hum, this.props.config.sensors.bme280.h) + '%, ' +
                                this.pres(this.props.data.bme280.pres, this.props.config.sensors.bme280.p) + text.get('mm', lang))}
                            </div>
                            <div className="mt-1">BMP180</div>
                            <div className="indication">{this.props.config.hasOwnProperty('sensors') && (
                                this.temp(this.props.data.bmp180.temp, this.props.config.sensors.bmp180.t) + '°C, ' +
                                this.pres(this.props.data.bmp180.pres, this.props.config.sensors.bmp180.p) + text.get('mm', lang))}
                            </div>
                            <div className="mt-1">SHT21</div>
                            <div className="indication">{this.props.config.hasOwnProperty('sensors') && (
                                this.temp(this.props.data.sht21.temp, this.props.config.sensors.sht21.t) + '°C, ' +
                                this.hum(this.props.data.sht21.hum, this.props.config.sensors.sht21.h) + '%')}
                            </div>
                            <div className="mt-1">DHT22</div>
                            <div className="indication">{this.props.config.hasOwnProperty('sensors') && (
                                this.temp(this.props.data.dht22.temp, this.props.config.sensors.dht22.t) + '°C, ' +
                                this.hum(this.props.data.dht22.hum, this.props.config.sensors.dht22.t) + '%')}
                            </div>
                            <div className="mt-1">DS18B20</div>
                            <div className="indication">{this.props.config.hasOwnProperty('sensors') && (
                                this.temp(this.props.data.ds18b20.temp, this.props.config.sensors.ds18b20.t) + '°C')}
                            </div>
                            <div className="mt-1">MAX44009</div>
                            <div className="indication">{this.props.config.hasOwnProperty('sensors') && (
                                this.light(this.props.data.max44009.light, this.props.config.sensors.max44009.l) + text.get('lux', lang))}
                            </div>
                            <div className="mt-1">BH1750</div>
                            <div className="indication">{this.props.config.hasOwnProperty('sensors') && (
                                this.light(this.props.data.bh1750.light, this.props.config.sensors.bh1750.l) + text.get('lux', lang))}
                            </div>
                            <div className="mt-1">{text.get('analogInput', lang)}</div>
                            <div className="indication">{this.props.config.hasOwnProperty('sensors') && (
                                this.voltage(this.props.data.analog.volt, this.props.config.sensors.analog.v) + text.get('v', lang))}
                            </div>   
                        </div>
                    </div></>}
                </div>
            </div>
        </>);
    }
};

export default Status;