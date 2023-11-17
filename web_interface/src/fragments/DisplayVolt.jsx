import Save from "../pages/Save";
import Languages from "../Languages";
import SelectInput from "./SelectInput";

class DisplayVolt extends Save {
    constructor(props) {
        super(props);
        this.changedConfig = this.changedConfig.bind(this);
    }
    
    render() {
        let text = new Languages();
        let lang = this.props.config.lang;
        let fields = [];
        for(let i=1; i<=8; i++) fields.push(text.get('field', lang) + ' ' + i);
        let sensorsVolt = ['--', "Thingspeak"];
        let vBatTypes = [text.get('batteryVoltage', lang), text.get('batteryPercentage', lang)];

        return (
            <div className="border border-secondary rounded p-3 disp-card h-100">
                <h5>{text.get('voltage', lang)}</h5>
                <SelectInput value={`display.source.volt.sens`}
                    label={text.get('dataSource', lang)}
                    options={sensorsVolt}
                    config={this.props.config} 
                    changedConfig={this.props.changedConfig}
                />

                {/* Thingspeak */}
                {this.props.config.display.source.volt.sens == 1 && <><hr />
                    <SelectInput value={`display.source.volt.thing`}
                        label={text.get('field', lang)}
                        options = {fields}
                        config={this.props.config} 
                        changedConfig={this.props.changedConfig}
                    />
                </>}

                {/* Battery data type */}
                {this.props.config.display.source.volt.sens > 0 &&
                    <SelectInput value={`display.source.volt.volt`}
                        label={text.get('sensorType', lang)}
                        options = {vBatTypes}
                        config={this.props.config} 
                        changedConfig={this.props.changedConfig}
                    />
                }
            </div>
        )
    }
};

export default DisplayVolt;