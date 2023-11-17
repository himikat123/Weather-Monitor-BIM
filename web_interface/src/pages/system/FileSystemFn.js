import Save from "../Save";
import Languages from "../../Languages";
import { closeMenu } from "../../menu/closeMenu";
import axios from 'axios';

class FileSystemFn extends Save {
    constructor(props) {
        super(props);
        this.state = {
            dir: '/',
            folders: [],
            fileSelected: '',
            fileData: '',
            renameFile: '',
            renameName: '',
            percentage: '',
            menuOpen: 1
        }

        this.upload = this.upload.bind(this);
        this.selectFile = this.selectFile.bind(this);
        this.openBtn = this.openBtn.bind(this);
        this.downloadBtn = this.downloadBtn.bind(this);
        this.renameBtn = this.renameBtn.bind(this);
        this.deleteBtn = this.deleteBtn.bind(this);
        this.changedName = this.changedName.bind(this);
        this.renameCancel = this.renameCancel.bind(this);
        this.renameOk = this.renameOk.bind(this);
        this.createFolderList = this.createFolderList.bind(this);
    }

    /**
    * Props have been updated 
    */
    componentDidUpdate() {
        if(this.state.menuOpen) {
            closeMenu();
            this.setState({ menuOpen: 0 });
        }
        this.createFolderList();
    }

    /**
     * Upload button
     */
    async upload() {
        this.props.uploading(true);
        let formData = new FormData();
        formData.append("file", document.querySelector('#uploadFile').files[0]);
    
        const onUploadProgress = (event) => {
            const percentage = Math.round((100 * event.loaded) / event.total);
            this.setState({ percentage: percentage + '%'});
            if(percentage > 99) this.props.uploading(false);
        };

        try {
            let origin = window.location.origin;
            let href = [origin.split(':')[0], origin.split(':')[1]].join(':');
            await axios.post(`${href}/esp/fileUpload`, formData, {
                headers: {
                    "Content-Type": "multipart/form-data",
                }, 
                onUploadProgress
            });
        } catch (error) { }
    }

    /**
     * Select a file in the list
     */
    selectFile(name) {
        this.setState({ fileSelected: name });
    }

    /**
     * Open button
     */
    openBtn() {
        if(this.state.fileSelected == '..') {                           // go to parrent directory
            let d = this.state.dir.match(/(.+|^)\/(.+\/)$/);
            if(d) this.setState({
                folders: [], 
                fileSelected: '',
                dir: this.state.dir.replace(d[2], '')
            });
        }
        else if(this.state.folders.includes(this.state.fileSelected)) { // folder open
            this.setState({folders: [], dir: `${this.state.dir}${this.state.fileSelected}/`});
        }
        else {                                                          // file open
            let filename = this.state.fileSelected;
            if(filename.endsWith('.html.gz') || 
               filename.endsWith('.json') ||
               filename.endsWith('.jpg')
            ) {
                fetch(window.location.origin + filename)
                .then(res => res.text())
                .then(
                    result => {
                        this.setState({ fileData: result });
                        document.getElementById('fileViewerOpen').click();
                    }
                )
            }
            else document.getElementById('fileViewerOpen').click();
        }
    }

    /**
     * File download button
     */
    downloadBtn() {
        document.getElementById(this.state.fileSelected).click();
    }

    /**
     * File rename button
     */
    renameBtn() {
        this.setState({ 
            renameFile: this.state.fileSelected,
            renameName: this.state.fileSelected
        });
    }

    /**
     * File was renamed
     */
    changedName(event) {
        this.setState({ renameName: event.target.value });
    }

    /**
     * Rename has been canceled
     */
    renameCancel() {
        this.setState({ 
            renameFile: '',
            renameName: ''
        });
    }

    /**
     * Rename has been confirmed
     */
    renameOk() {
        let text = new Languages();
        let lang = this.props.lang;
        this.setState({ renameFile: '' });
        for(let file in this.props.data.fs.list) {
            if(this.props.data.fs.list[file].name == this.state.renameName) { 
                if(file == this.state.fileSelected) return;
                else {
                    alert(text.get('FileAlreadyExists', lang).replace(new RegExp("XXX", "g"), this.state.renameName));
                    return;
                }
            }
        }
        let origin = window.location.origin;
        let href = [origin.split(':')[0], origin.split(':')[1]].join(':');
        axios({
            method: 'post',
            url: `${href}/esp/rename`,
            data: `old=${this.state.fileSelected}&new=${this.state.dir + this.state.renameName}`
        })
    }

    /**
     * File delete button
     */
    deleteBtn() {
        let text = new Languages();
        let lang = this.props.config.lang;
        let file = this.state.fileSelected;
        if(window.confirm(text.get('confirmDeletionOfTheFile', lang).replace(new RegExp("XXX", "g"), file))) {
            let origin = window.location.origin;
            let href = [origin.split(':')[0], origin.split(':')[1]].join(':');
            axios({
                method: 'post',
                url: `${href}/esp/delete`,
                data: `file=${file}`
            })
        }
    }

    /**
     * Сheck if directory belongs to current directory
     */
    createFolderList() {
        if(this.props.data.hasOwnProperty('fs')) {
            this.props.data.fs.list.map(x => {
                let directory = x[0].replace(this.state.dir, '');
                if(directory.includes('/') && !directory.startsWith('/')) {
                    let fld = this.state.folders;
                    let dirname = directory.split('/')[0];
                    if(!fld.includes(dirname)) fld.push(dirname);
                    if(this.state.folders != fld) this.setState({folders: fld});
                }
            })
            let fld = this.state.folders;
            if(!fld.includes('..')) fld.unshift('..');
            if(this.state.folders != fld) this.setState({folders: fld});
        }
    } 

    /**
     * Сheck if the file belongs to current directory
     */
    isFileFromCurrentDir(file) {
        let filename = file.replace(this.state.dir, '');
        if(filename.includes('/')) return false;
        else return true;
    }
};

export default FileSystemFn;