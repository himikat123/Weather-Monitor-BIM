import Save from "../pages/Save";
import Languages from "../Languages";
import SelectInput from "./SelectInput";

class DisplayBat extends Save {
    constructor(props) {
        super(props);
        this.changedConfig = this.changedConfig.bind(this);
    }
    
    render() {
        let text = new Languages();
        let lang = this.props.config.lang;
        let fields = [];
        for(let i=1; i<=8; i++) fields.push(text.get('field', lang) + ' ' + i);
        let sensorsBat = ['--', "Thingspeak"];

        return (
            <div className="border border-secondary rounded p-3 disp-card h-100">
                <h5>{text.get('batterySymbol', lang)}</h5>
                <SelectInput value={`display.source.bat.sens`}
                    label={text.get('dataSource', lang)}
                    options={sensorsBat}
                    config={this.props.config} 
                    changedConfig={this.props.changedConfig}
                />

                {/* Thingspeak */}
                {this.props.config.display.source.bat.sens == 1 && <><hr />
                    <SelectInput value={`display.source.bat.thing`}
                        label={text.get('field', lang)}
                        options = {fields}
                        config={this.props.config} 
                        changedConfig={this.props.changedConfig}
                    />
                </>}
            </div>
        )
    }
};

export default DisplayBat;