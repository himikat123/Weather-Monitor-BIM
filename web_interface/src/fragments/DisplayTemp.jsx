import Save from "../pages/Save";
import Languages from "../Languages";
import SelectInput from "./SelectInput";

class DisplayTemp extends Save {
    constructor(props) {
        super(props);
        this.changedConfig = this.changedConfig.bind(this);
    }
    
    render() {
        let text = new Languages();
        let lang = this.props.config.lang;
        let fields = [];
        for(let i=1; i<=8; i++) fields.push(text.get('field', lang) + ' ' + i);
        let sensorsTemp = ["--", text.get('forecast', lang), "Thingspeak", "BME280", "BMP180", "SHT21", "DHT22", "DS18B20"];
        let sensType = this.props.place == 'In' ? 
            this.props.config.display.source.tempIn.sens : 
            this.props.config.display.source.tempOut.sens;

        return (
            <div className="border border-secondary rounded p-3 disp-card h-100">
                <div className="w-100">
                    <h5>{text.get(`temperature${this.props.place}`, lang)}</h5>
                    <SelectInput value={`display.source.temp${this.props.place}.sens`}
                        label={text.get('dataSource', lang)}
                        options={sensorsTemp}
                        config={this.props.config} 
                        changedConfig={this.props.changedConfig}
                    />

                    {/* Thingspeak */}
                    {sensType == 2 && <><hr />
                        <SelectInput value={`display.source.temp${this.props.place}.thing`}
                            label={text.get('field', lang)}
                            options = {fields}
                            config={this.props.config} 
                            changedConfig={this.props.changedConfig}
                        />
                    </>}
                </div>
            </div>
        )
    }
};

export default DisplayTemp;