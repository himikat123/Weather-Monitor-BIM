import Save from "../pages/Save";
import Languages from "../Languages";
import SelectInput from "./SelectInput";

class DisplayHum extends Save {
    constructor(props) {
        super(props);
        this.changedConfig = this.changedConfig.bind(this);
    }

    render() {
        let text = new Languages();
        let lang = this.props.config.lang;
        let fields = [];
        for(let i=1; i<=8; i++) fields.push(text.get('field', lang) + ' ' + i);
        let sensorsHum = ["--", text.get('forecast', lang), "Thingspeak", "BME280", "SHT21", "DHT22"];
        let sensType = this.props.place == 'In' ? 
            this.props.config.display.source.humIn.sens : 
            this.props.config.display.source.humOut.sens;

        return (
            <div className="border border-secondary rounded p-3 disp-card h-100">
                <div className="w-100">
                    <h5>{text.get(`humidity${this.props.place}`, lang)}</h5>
                    <SelectInput value={`display.source.hum${this.props.place}.sens`}
                        label={text.get('dataSource', lang)}
                        options={sensorsHum}
                        config={this.props.config} 
                        changedConfig={this.props.changedConfig}
                    />

                    {/* Thingspeak */}
                    {sensType == 2 && <><hr />
                        <SelectInput value={`display.source.hum${this.props.place}.thing`}
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

export default DisplayHum;