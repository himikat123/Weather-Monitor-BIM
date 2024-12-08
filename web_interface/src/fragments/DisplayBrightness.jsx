import Save from "../pages/Save";
import Languages from "../Languages";
import SelectInput from "./SelectInput";
import RangeInput from "./RangeInput";
import TimeInput from "./TimeInput";

class DisplayBrightness extends Save {
    constructor(props) {
        super(props);
        this.changedConfig = this.changedConfig.bind(this);
        this.setBrightDay = this.setBrightDay.bind(this);
        this.setBrightNight = this.setBrightNight.bind(this);
        this.setSensitivity = this.setSensitivity.bind(this);
    }

    setBrightDay() {
        this.sendGetRequest('bright', `bright=${this.props.config.display.brightness.day}`);
    }

    setBrightNight() {
        this.sendGetRequest('bright', `bright=${this.props.config.display.brightness.night}`);
    }

    setSensitivity() {
        this.sendGetRequest('sensitivity', `bright=${this.props.config.display.sensitivity}`);
    }
    
    render() {
        let text = new Languages();
        let lang = this.props.config.lang;

        let displayBrightness = [
            text.get('bySunriseAndSunset', lang),
            text.get('byLightSensor', lang),
            text.get('byTime', lang),
            text.get('constantBrightness', lang)
        ];

        let displayLightSensors = [
            text.get('analogInput', lang),
            'MAX44009', 
            'BH1750'
        ];

        let displayLightData = [
            "analog",
            "max44009", 
            "bh1750"
        ];

        let displayLightDataType = [
            'volt', 
            "light", 
            "light"
        ];

        let sensorNum = this.getVal(this.props.config, 'display.lightSensor');
        let senosrData = '--';

        if(this.props.data.hasOwnProperty(displayLightData[sensorNum])) {
            senosrData = this.props.data[displayLightData[sensorNum]][displayLightDataType[sensorNum]];
            if(sensorNum == 0 && (senosrData < 0 || senosrData > 5)) senosrData = '--';
            if(sensorNum >= 1 && (senosrData < 0 || senosrData > 120000)) senosrData = '--';
        }

        let brightness = sensorNum == 0 ? senosrData * 30 : senosrData;
        brightness *= this.props.config.display.sensitivity / 20;
        if(brightness < 1) brightness = 1;
        if(brightness > 100) brightness = 100;
        brightness = Math.round(brightness);

        return (
            <div className="border border-secondary rounded p-3 disp-card h-100">
                <h5>{text.get('brightness', lang)}</h5>
                <SelectInput value={'display.brightMethod'}
                    label={text.get('displayBrightness', lang)}
                    options={displayBrightness} 
                    config={this.props.config} 
                    changedConfig={this.changedConfig}
                />

                {/* Brightness Auto */}
                {this.props.config.display.brightMethod == 0 && <> 
                    <RangeInput value={'display.brightness.day'}
                        label={text.get('daytimeBrightness', lang)} 
                        min="1"
                        max="100"
                        limitMin="1"
                        limitMax="100"
                        step="1"
                        config={this.props.config} 
                        changedConfig={this.changedConfig}
                        callback={this.setBrightDay}
                    />

                    <RangeInput value={'display.brightness.night'}
                        label={text.get('nightBrightness', lang)} 
                        min="1"
                        max="100"
                        limitMin="1"
                        limitMax="100"
                        step="1"
                        config={this.props.config} 
                        changedConfig={this.changedConfig}
                        callback={this.setBrightNight}
                    />
                </>}

                {/* Brightness by ambient light sensor */}
                {this.props.config.display.brightMethod == 1 && <> 
                    <SelectInput value={'display.lightSensor'}
                        label={text.get('sensorType', lang)}
                        options={displayLightSensors} 
                        config={this.props.config} 
                        changedConfig={this.changedConfig}
                    />

                    <div className="float-end my-2">
                        {text.get(this.props.config.display.lightSensor > 0 ? 'ambientLight' : 'voltage', lang)}
                            : <span className="text-primary indication me-3">
                            {senosrData} {text.get(this.props.config.display.lightSensor > 0 ? 'lux' : 'v', lang)}
                        </span>
                    </div>

                    <div className="mt-4">
                        <RangeInput value={'display.sensitivity'}
                            label={text.get('sensorSensitivity', lang)} 
                            min="1"
                            max="100"
                            limitMin="1"
                            limitMax="100"
                            step="1"
                            config={this.props.config} 
                            changedConfig={this.changedConfig}
                            callback={this.setSensitivity}
                        />
                        <div className="float-end my-4">
                            {text.get('brightness', lang)}: <span className="text-primary indication me-3">
                                {isNaN(brightness) ? '--' : brightness}%
                            </span>
                        </div>
                    </div>
                </>}

                {/* Brightness by time */}
                {this.props.config.display.brightMethod == 2 && <> 
                    <TimeInput value={'display.dayTime'}
                        label={text.get('dayMode', lang)}
                        placeholder=""
                        config={this.props.config}
                        changedConfig={this.changedConfig}
                    />

                    <div className="mt-3">
                        <RangeInput value={'display.brightness.day'} 
                            min="1"
                            max="100"
                            limitMin="1"
                            limitMax="100"
                            step="1"
                            config={this.props.config} 
                            changedConfig={this.changedConfig}
                            callback={this.setBrightDay}
                        />
                    </div>
                                
                    <TimeInput value={'display.nightTime'}
                        label={text.get('nightMode', lang)}
                        placeholder=""
                        config={this.props.config}
                        changedConfig={this.changedConfig}
                    />

                    <div className="mt-3">
                        <RangeInput value={'display.brightness.night'} 
                            min="1"
                            max="100"
                            limitMin="1"
                            limitMax="100"
                            step="1"
                            config={this.props.config} 
                            changedConfig={this.changedConfig}
                            callback={this.setBrightNight}
                        />
                    </div>
                </>}

                {/* Brightness constant */}
                {this.props.config.display.brightMethod == 3 && 
                    <RangeInput value={'display.brightness.day'}
                        label={text.get('brightness', lang)} 
                        min="1"
                        max="100"
                        limitMin="1"
                        limitMax="100"
                        step="1"
                        config={this.props.config} 
                        changedConfig={this.changedConfig}
                        callback={this.setBrightDay}
                    />
                }
            </div>
        )
    }
};

export default DisplayBrightness;