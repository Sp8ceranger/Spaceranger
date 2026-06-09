//% color=#ff8800 icon="\\uf1d9" block="Arduino Link"

namespace ArduinoLink {

    //% block="initialiser communication Arduino"

    export function init() {

        serial.redirect(

            SerialPin.P0,

            SerialPin.P1,

            BaudRate.BaudRate9600

        )

    }

    //% block="envoyer à Arduino %msg"

    export function send(msg: string) {

        serial.writeLine(msg)

    }

    //% block="lire message Arduino"

    export function receive(): string {

        return serial.readLine()

    }

}