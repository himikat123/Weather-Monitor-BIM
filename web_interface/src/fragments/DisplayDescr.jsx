import Save from "../pages/Save";
import Languages from "../Languages";
import SelectInput from "./SelectInput";

class DisplayDescr extends Save {
    constructor(props) {
        super(props);
        this.changedConfig = this.changedConfig.bind(this);
    }
    
    render() {
        let text = new Languages();
        let lang = this.props.config.lang;

        return (
            <div className="border border-secondary rounded p-3 disp-card h-100">
                <h5>{text.get('additionalDescription', lang)}</h5>
                <SelectInput value={`display.source.descr`}
                    label={text.get('dataSource', lang)}
                    options={['--', text.get('comfortLevel', lang)]}
                    config={this.props.config} 
                    changedConfig={this.props.changedConfig}
                />
            </div>
        )
    }
};

export default DisplayDescr;