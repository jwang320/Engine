export default class Utils {
    /**
     * @param {HTMLElement} elem parent
     */
    static deleteChildren = elem => {
        while (elem.firstChild) {
            elem.removeChild(elem.firstChild);
        }
    };
}