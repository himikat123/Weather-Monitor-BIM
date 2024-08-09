import ComfortFn from "./ComfortFn";
import Languages from "../../Languages";
import PageHeader from "../../fragments/PageHeader";
import RangeInput from "../../fragments/RangeInput";
import SelectInput from "../../fragments/SelectInput";
import Menu from "../../menu/Menu";
import SaveRestart from "../SaveRestart";

class Comfort extends ComfortFn {
    render() {
        let text = new Languages();
        let lang = this.props.config.lang;

        let temperatures = [
            "--", text.get('forecast', lang), "Thingspeak", "BME280", "BMP180", "SHT21", "DHT22", "DS18B20"
        ];

        let humidities  = [
            "--", text.get('forecast', lang), "Thingspeak", "BME280", "SHT21", "DHT22"
        ];

        let fields = [];
        for(let i=1; i<=8; i++) fields.push(text.get('field', lang) + ' ' + i);

        return(<>
            <Menu language={lang} />
            
            <div className="d-flex justify-content-center">
                <div className="content m-2 m-lg-5">
                    <PageHeader h="2" text={text.get('comfort', lang)} />

                    {this.props.config.hasOwnProperty('comfort') && 
                    <div className="row">
                        <div className="col col-12 col-md-6 p-4">
                            <div className="border border-secondary rounded p-3 sensor">
                                <SelectInput value={'comfort.temp.source'}
                                    label={text.get('tempSource', lang)}
                                    options={temperatures}
                                    config={this.props.config} 
                                    changedConfig={(this.props.changedConfig)}
                                />

                                {this.props.config.comfort.temp.source == 2 &&
                                    <SelectInput value={'comfort.temp.thing'}
                                        label={text.get('field', lang)}
                                        options={fields}
                                        config={this.props.config} 
                                        changedConfig={(this.props.changedConfig)}
                                    />
                                }

                                {this.props.config.comfort.temp.source > 0 && <>
                                    <RangeInput value={'comfort.temp.max'}
                                        label={text.get('tempMax', lang)} 
                                        min="-50"
                                        max="100"
                                        limitMin={this.props.config.comfort.temp.min}
                                        limitMax="100"
                                        step="1"
                                        config={this.props.config} 
                                        changedConfig={this.changedConfig}
                                        units={"°C"}
                                    />
                                    <RangeInput value={'comfort.temp.min'}
                                        label={text.get('tempMin', lang)} 
                                        min="-50"
                                        max="100"
                                        limitMin="-50"
                                        limitMax={this.props.config.comfort.temp.max}
                                        step="1"
                                        config={this.props.config} 
                                        changedConfig={this.changedConfig}
                                        units={"°C"}
                                    />
                                </>}
                            </div>
                        </div>
                        <div className="col col-12 col-md-6 p-4">
                            <div className="border border-secondary rounded p-3 sensor">
                                <SelectInput value={'comfort.hum.source'}
                                    label={text.get('humSource', lang)}
                                    options={humidities}
                                    config={this.props.config} 
                                    changedConfig={(this.props.changedConfig)}
                                />

                                {this.props.config.comfort.hum.source == 2 &&
                                    <SelectInput value={'comfort.hum.thing'}
                                        label={text.get('field', lang)}
                                        options={fields}
                                        config={this.props.config} 
                                        changedConfig={(this.props.changedConfig)}
                                    />
                                }

                                {this.props.config.comfort.hum.source > 0 && <>
                                    <RangeInput value={'comfort.hum.max'}
                                        label={text.get('humMax', lang)} 
                                        min="0"
                                        max="100"
                                        limitMin={this.props.config.comfort.hum.min}
                                        limitMax="100"
                                        step="1"
                                        config={this.props.config} 
                                        changedConfig={this.changedConfig}
                                        units={"%"}
                                    />
                                    <RangeInput value={'comfort.hum.min'}
                                        label={text.get('humMin', lang)} 
                                        min="0"
                                        max="100"
                                        limitMin="0"
                                        limitMax={this.props.config.comfort.hum.max}
                                        step="1"
                                        config={this.props.config} 
                                        changedConfig={this.changedConfig}
                                        units={"%"}
                                    />
                                </>}
                            </div>
                        </div>
                    </div>}
                </div>
            </div>

            <SaveRestart config={this.props.config} save="1" restart="1" />
        </>)
    };
};

export default Comfort;