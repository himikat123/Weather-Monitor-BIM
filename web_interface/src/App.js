import AppFn from "./AppFn";
import { BrowserRouter as Router, Route, Routes } from "react-router-dom";
import Status from "./pages/status/Status";
import Connect from "./pages/network/Connect";
import AccessPoint from "./pages/network/AccessPoint";
import Sensors from "./pages/source/Sensors";
import Weather from "./pages/source/Weather";
import Clock from "./pages/clock/Clock";
import Display from "./pages/display/Display";
import Comfort from "./pages/comfort/Comfort";
import Send from "./pages/cloud/Send";
import Receive from "./pages/cloud/Receive";
import Backup from "./pages/system/Backup";
import Default from "./pages/system/Default";
import Firmware from "./pages/system/Firmware";
import FileSystem from "./pages/system/FileSystem";
import Username from "./pages/account/Username";
import Userpass from "./pages/account/Userpass";
import Language from "./pages/account/Language";
import Login from "./pages/account/Login";
import { Navigate } from 'react-router-dom';

class App extends AppFn {
    render() {
        return (
            <Router>
                {this.state.data.hasOwnProperty('state') && this.state.data.state == 'LOGIN' && 
                window.location.pathname != '/login' && <Navigate to='/login' />}
                {this.state.config.hasOwnProperty('state') && this.state.config.state == 'LOGIN' && 
                window.location.pathname != '/login' && <Navigate to='/login' />}

                {/* If the host informs about the need to log in */}
                {this.state.config.hasOwnProperty('login') && 
                    (this.state.path != '/login' ? window.location.assign("/login") : '')
                }

                {/* Show pages */}
                <Routes>
                    <Route exact path="/" element={
                        <Status config={this.state.config} 
                            data={this.state.data}
                            newVersion={this.state.newVersion}
                            gitVersion={this.state.gitVersion}
                        />
                    } />
                    <Route exact path="/connect" element={
                        <Connect config={this.state.config} 
                            changedConfig={this.changedConfig}
                            data={this.state.data} 
                        />
                    } />
                    <Route exact path="/accesspoint" element={
                        <AccessPoint config={this.state.config} 
                            changedConfig={this.changedConfig} 
                        />
                    } />
                    <Route exact path="/sensors" element={
                        <Sensors config={this.state.config} 
                            changedConfig={this.changedConfig}
                            data={this.state.data} 
                        />
                    } />
                    <Route exact path="/weather" element={
                        <Weather config={this.state.config} 
                            changedConfig={this.changedConfig} 
                        />
                    } />
                    <Route exact path='/clock' element={
                        <Clock config={this.state.config} 
                            changedConfig={this.changedConfig} 
                        />
                    } />
                    <Route exact path='/display' element={
                        <Display config={this.state.config} 
                            changedConfig={this.changedConfig}
                            data={this.state.data}
                        />
                    } />
                    <Route exact path='/comfort' element={
                        <Comfort config={this.state.config} 
                            changedConfig={this.changedConfig} 
                        />
                    } />
                    <Route exact path='/send' element={
                        <Send config={this.state.config} 
                            changedConfig={this.changedConfig}
                            data={this.state.data}
                        />
                    } />
                    <Route exact path='/receive' element={
                        <Receive config={this.state.config} 
                            changedConfig={this.changedConfig}
                        />
                    } />
                    <Route exact path='/backup' element={
                        <Backup config={this.state.config} />
                    } />
                    <Route exact path='/default' element={
                        <Default config={this.state.config} />
                    } />
                    <Route exact path='/firmware' element={
                        <Firmware config={this.state.config} />
                    } />
                    <Route exact path='/filesystem' element={
                        <FileSystem config={this.state.config}
                            data={this.state.data}
                            uploading={e => this.setState({uploading: e})}
                        />
                    } />

                    <Route exact path='/username' element={
                        <Username config={this.state.config} 
                            changedConfig={this.changedConfig}
                        />
                    } />
                    <Route exact path='/userpass' element={
                        <Userpass config={this.state.config} 
                            pass={this.changedPass}
                        />
                    } />
                    <Route exact path='/lang' element={
                        <Language config={this.state.config} 
                            changedConfig={this.changedConfig}
                        />
                    } />
                    <Route exact path='/login' element={
                        <Login lang={this.state.config.lang}
                            data={this.state.data}
                            error={this.loginError} 
                        />
                    } />
                </Routes>
            </Router>
        )
    };
};

export default App;