import Actor from "./Actor.js";

export default class RenderObject {
    /**
     * @param {Actor} attachedObject
     */
    constructor(attachedObject) {
        this.object = attachedObject;
        attachedObject.renderObject = this;
        this.geometry = new THREE.BoxGeometry(attachedObject.scale.x, attachedObject.scale.y, attachedObject.scale.z);
        //this.material = new THREE.MeshBasicMaterial({ color: 0x00ff00 });
        let color;
        switch (attachedObject.objectType) {
            case 2:
                color = 0x3310FF;
                break;
            default:
                color = 0x00ff00;
                break;
        }
        this.material = new THREE.MeshPhongMaterial({
            color: color, specular: color, shininess: 1, opacity: 1
        });
        this.cube = new THREE.Mesh(this.geometry, this.material);

        switch (attachedObject.objectType) {
            case 2:
                this.cube.castShadow = true;
                break;
            default:
                this.cube.receiveShadow = true;
                break;
        }
        this.cube.name = attachedObject.globalId;
    }
}