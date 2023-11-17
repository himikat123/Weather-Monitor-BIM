import FileSystemFn from "./FileSystemFn";
import Languages from "../../Languages";
import PageHeader from "../../fragments/PageHeader";
import ModalFileViewer from "../../modals/ModalFileViewer";
import Menu from "../../menu/Menu";
import SaveRestart from "../SaveRestart";

class FileSystem extends FileSystemFn {
    render() {
        let text = new Languages();
        let lang = this.props.config.lang;
        
        return (<>
            <Menu language={lang} />
            
            <div className="d-flex justify-content-center">
                <div className="content m-2 m-lg-5">
                    <PageHeader h="2" text={text.get('fileSystem', lang)} />

                    <div className="row justify-content-center">
                        <div className="col border border-secondary rounded m-4 p-3">

                            {/* File upload */}
                            <div className="row">
                                <div className="my-1 mt-sm-0 col-12 col-sm-6 col-xl-3">
                                    <input id="uploadFile" name="upload" className="form-control mt-3" type="file"/>
                                </div>
                                <div className="my-1 mt-sm-3 col-12 col-sm-6 col-md-3">
                                    <button className="btn btn-secondary w-100" onClick={this.upload}>
                                        {text.get('upload', lang)} {this.state.percentage}
                                    </button>
                                </div>
                            </div><hr />

                            {/* Control buttons */}
                            <div className="row">
                                {/* Open button */}
                                <div className="my-1 col-12 col-sm-6 col-md-3">
                                    <button className="btn btn-secondary w-100" 
                                      onClick={this.openBtn}
                                      disabled={this.state.fileSelected == ''}
                                    >
                                        {text.get('open', lang)}
                                    </button>
                                </div>
                                {/* Download button */}
                                <div className="my-1 col-12 col-sm-6 col-md-3">
                                    <button className="btn btn-secondary w-100" 
                                      onClick={this.downloadBtn}
                                      disabled={this.state.folders.includes(this.state.fileSelected) || this.state.fileSelected == ''}
                                    >
                                        {text.get('download', lang)}
                                    </button>
                                </div>
                                {/* Rename button */}
                                <div className="my-1 col-12 col-sm-6 col-md-3">
                                    <button className="btn btn-secondary w-100" 
                                      onClick={this.renameBtn}
                                      disabled={this.state.folders.includes(this.state.fileSelected) || this.state.fileSelected == ''}
                                    >
                                        {text.get('rename', lang)}
                                    </button>
                                </div>
                                {/* Delete button */}
                                <div className="my-1 col-12 col-sm-6 col-md-3">
                                    <button className="btn btn-secondary w-100" 
                                      onClick={this.deleteBtn}
                                      disabled={this.state.folders.includes(this.state.fileSelected) || this.state.fileSelected == ''}
                                    >
                                        {text.get('delete', lang)}
                                    </button>
                                </div>
                            </div><hr />

                            {/* List of files */}
                            {this.props.data.hasOwnProperty('fs') &&
                                <div className="mt-3">
                                    <div className="d-flex justify-content-between flex-sm-column-reverse flex-md-row">
                                        <div className="mt-3">
                                            <input className="form-control" 
                                                   value={
                                                        this.state.dir.length == 1 ?
                                                        '/' :
                                                        this.state.dir.slice(0, -1)
                                                    } 
                                                   readOnly 
                                            />
                                        </div>
                                        <div className="text-end me-3 mt-4">
{this.props.data.fs.free}{text.get('kb', lang)} {text.get('freeOf', lang)} {this.props.data.fs.total}{text.get('kb', lang)}
                                        </div>
                                    </div>
                                    <div className="row mb-3 mt-3">
                                        <div className="col-6 text-center border border-secondary">
                                            {text.get('name', lang)}
                                        </div>
                                        <div className="col-3 text-center border border-secondary">
                                            {text.get('type', lang)}
                                        </div>
                                        <div className="col-3 text-center border border-secondary">
                                            {text.get('size', lang)}
                                        </div>
                                    </div>

                                    {/* Folders */}
                                    {this.createFolderList()}
                                    {this.state.folders.map(folder => {
                                        return <div key={folder} 
                                          className={"zebra row mb-1 hoverable" + (this.state.fileSelected == folder ? " active" : "")}
                                          onClick={() => this.selectFile(folder)}
                                          onDoubleClick={() => this.openBtn()}
                                        >
                                           <div className="col-6 text-info">
                                               {folder}
                                           </div>
                                           <div className="col-3 text-info">{folder != '..' && text.get('fileFolder', lang)}</div>
                                           <div className="col-3"></div>
                                       </div>
                                    })}

                                    {/* Files */}
                                    {this.props.data.fs.list.map((x, i) => {
                                        return (this.isFileFromCurrentDir(x[0]) && <div key={i}
                                          className={"zebra row mb-1 hoverable" + (this.state.fileSelected == x[0] ? " active" : "")}
                                          onClick={() => this.selectFile(x[0])}
                                          onDoubleClick={() => this.openBtn()}
                                        >
                                            <div className="col-6 text-info">

                                                {/* File download */}
                                                <a id={x[0]} href={x[0]} download></a>

                                                {/* Filename */}
                                                {this.state.renameFile != x[0] && x[0].replace(this.state.dir, '')}

                                                {/* File rename */}
                                                {this.state.renameFile == x[0] && <>
                                                    <div className="input-group">
                                                    <input type="text"
                                                        data-index={i} 
                                                        className="form-control rename-field"
                                                        maxLength="32"
                                                        defaultValue={x[0].replace(this.state.dir, '')}
                                                        onChange={this.changedName}
                                                        autoFocus
                                                    />
                                                    <button className="btn btn-success rename-btn" 
                                                        onClick={this.renameOk}
                                                        title={text.get('save', lang)}
                                                    >✓</button>
                                                    <button className="btn btn-danger rename-btn" 
                                                        onClick={this.renameCancel}
                                                        title={text.get('cancel', lang)}
                                                    >×</button>
                                                    </div>
                                                </>}
                                            </div>
                                            <div className="col-3 text-info">
                                                {x[0].split('.')[1].toUpperCase()} 
                                            </div>
                                            <div className="col-3 text-info">
                                                {x[1]}{text.get('kb', lang)}
                                            </div>
                                        </div>)}
                                    )}

                                    {/* Modal file viewer */}
                                    <button id="fileViewerOpen" 
                                        style={{ display: 'none'}} 
                                        data-bs-toggle="modal" 
                                        data-bs-target={"#fileviewer"} 
                                    />
                                    <ModalFileViewer lang={lang} 
                                        fileName={this.state.fileSelected} 
                                        data={this.state.fileData} 
                                    />
                                </div>
                            }
                        </div>
                    </div>
                </div>

            </div>

            <SaveRestart config={this.props.config} restart="1" />
        </>);
    }
};

export default FileSystem;