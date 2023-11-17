import DisplayFn from "./DisplayFn";
import Languages from "../../Languages";
import PageHeader from "../../fragments/PageHeader";
import DisplayBrightness from "../../fragments/DisplayBrightness";
import DisplayTemp from "../../fragments/DisplayTemp";
import DisplayHum from "../../fragments/DisplayHum";
import DisplayPres from "../../fragments/DisplayPres";
import DisplayVolt from "../../fragments/DisplayVolt";
import DisplayBat from "../../fragments/DisplayBat";
import DisplayDescr from "../../fragments/DisplayDescr";
import Menu from "../../menu/Menu";
import SaveRestart from "../SaveRestart";

class Display extends DisplayFn {
    render() {
        let text = new Languages();
        let lang = this.props.config.lang;

        return (<>
            <Menu language={lang} />
            
            <div className="d-flex justify-content-center">
                {this.props.config.hasOwnProperty('display') &&
                <div className="content m-2 m-lg-5">
                    <PageHeader h="2" text={text.get('display', lang)} />
            
                    <div className="row justify-content-center">

                        {/* Brightness */}
                        <div className="col-12 col-lg-6 col-xl-4 p-4">
                            <DisplayBrightness config={this.props.config} data={this.props.data} changedConfig={this.changedConfig} />
                        </div>

                        {/* Temperature inside */}
                        <div className="col-12 col-lg-6 col-xl-4 p-4">
                            <DisplayTemp place="In" config={this.props.config} changedConfig={this.changedConfig} />
                        </div>
                        
                        {/* Humidity inside */}
                        <div className="col-12 col-lg-6 col-xl-4 p-4">
                            <DisplayHum place="In" config={this.props.config} changedConfig={this.changedConfig} />
                        </div>
                        
                        {/* Battery voltage */}
                        <div className="col-12 col-lg-6 col-xl-4 p-4">
                            <DisplayVolt config={this.props.config} changedConfig={this.changedConfig} />
                        </div>

                        {/* Battery symbol */}
                        <div className="col-12 col-lg-6 col-xl-4 p-4">
                            <DisplayBat config={this.props.config} changedConfig={this.changedConfig} />
                        </div>

                        {/* Additional description */}
                        <div className="col-12 col-lg-6 col-xl-4 p-4">
                            <DisplayDescr config={this.props.config} changedConfig={this.changedConfig} />
                        </div>
                        
                        {/* Temperature outside */}
                        <div className="col-12 col-lg-6 col-xl-4 p-4">
                            <DisplayTemp place="Out" config={this.props.config} changedConfig={this.changedConfig} />
                        </div>
                        
                        {/* Humidity outside */}
                        <div className="col-12 col-lg-6 col-xl-4 p-4">
                            <DisplayHum place="Out" config={this.props.config} changedConfig={this.changedConfig} />
                        </div>
                        
                        {/* Pressure outside */}
                        <div className="col-12 col-lg-6 col-xl-4 p-4">
                            <DisplayPres config={this.props.config} changedConfig={this.changedConfig} />
                        </div>
                    </div>
                </div>}
            </div>

            <SaveRestart config={this.props.config} save="1" restart="1" />
        </>);
    };
}

export default Display;