import RenderObject from "./RenderObject.js";
import Vec4 from "./Vec4.js";

export default class Renderer {
    /** @type {RenderObject[]}*/
    renderObjects = [];

    /** @type {number} */
    focusObjectId = null;

    /** 
     *  @param {InfoBoard} infoBoard
     *  @param {Application} application
     * */
    constructor(infoBoard, application) {
        /** @type {Application} */
        this.application = application;
        this.scene = new THREE.Scene();
        // this.camera = new THREE.PerspectiveCamera(75, 800.0 / 600.0, 0.1, 200000);
        this.camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 200000);
        this.infoBoard = infoBoard;
        this.renderer = new THREE.WebGLRenderer();
        this.renderer.setSize(window.innerWidth, window.innerHeight);
        this.renderer.shadowMap.enabled = true;
        this.renderer.shadowMap.type = THREE.PCFSoftShadowMap;
        //this.renderer.setSize(800, 600);
        var directionalLight = new THREE.PointLight(0xffffff, 0.8, 10000);
        directionalLight.position.set(0, 100, 0);
        var light = new THREE.AmbientLight(0x404040); // soft white light 
        directionalLight.castShadow = true;
        directionalLight.shadow.mapSize.width = 512;  // default
        directionalLight.shadow.mapSize.height = 512; // default
        directionalLight.shadow.camera.near = 0.5;       // default
        directionalLight.shadow.camera.far = 500;      // default
        directionalLight.decay = 2;
        this.scene.add(light);
        this.scene.add(directionalLight);
        document.body.appendChild(this.renderer.domElement);
        var helper = new THREE.CameraHelper(this.camera);
        this.scene.add(helper);
        this.camera.up = new THREE.Vector3(0, 1, 0);
        this.camera.position.z = 10;
        this.animate();
    }

    animate = () => {
        if (this.closed) {
            return;
        }

        this.application.clearStaleObjects();

        for (let i = 0; i < this.renderObjects.length; ++i) {
            const obj = this.renderObjects[i].object;
            this.renderObjects[i].cube.position.x = obj.location.x;
            this.renderObjects[i].cube.position.y = obj.location.y;
            this.renderObjects[i].cube.position.z = obj.location.z;
            this.renderObjects[i].cube.quaternion.x = obj.orientation.x;
            this.renderObjects[i].cube.quaternion.y = obj.orientation.y;
            this.renderObjects[i].cube.quaternion.z = obj.orientation.z;
            this.renderObjects[i].cube.quaternion.w = obj.orientation.w;
            if (this.focusObjectId === obj.globalId) {
                this.camera.position.x = obj.location.x;
                this.camera.position.y = obj.location.y + 25;
                this.camera.position.z = obj.location.z;
                this.camera.up = new THREE.Vector3(0, 1, 0);
                const facing = obj.getFacing();
                const tFacing = new THREE.Vector3(obj.location.x + facing.x, obj.location.y + facing.y + 25, obj.location.z + facing.z);
                //console.log(tFacing);
                this.camera.lookAt(tFacing);
                //this.camera.lookAt(new THREE.Vector3(obj.facing.x, obj.facing.y, obj.facing.z));
                this.infoBoard.updateCameraPosition(new Vec4(this.camera.position.x, this.camera.position.y, this.camera.position.z, 1));
            }
        }

        requestAnimationFrame(this.animate);
        this.renderer.render(this.scene, this.camera);
    };

    /** @param {RenderObject} renderObject */
    addObject = renderObject => {
        this.scene.add(renderObject.cube);
        this.renderObjects.push(renderObject);
    }

    /** @param {RenderObject} renderObject */
    removeObject = renderObject => {
        if (renderObject) {
            this.scene.remove(renderObject.cube);
            var index = this.renderObjects.indexOf(renderObject);
            if (index !== -1) this.renderObjects.splice(index, 1);
        }
    }

    close() {
        this.closed = true;
        document.body.removeChild(this.renderer.domElement);
    }
}