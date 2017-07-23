/**
 *  Child Voltage Sensor
 *
 *  Copyright 2017 Daniel Ogorchock
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 *  in compliance with the License. You may obtain a copy of the License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0/**
 *  Child Voltage Sensor
 *
 *  Copyright 2017 Daniel Ogorchock
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 *  in compliance with the License. You may obtain a copy of the License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
 *  on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License
 *  for the specific language governing permissions and limitations under the License.
 *
 *  Change History:
 *
 *    Date        Who            What
 *    ----        ---            ----
 *    2017-04-19  Dan Ogorchock  Original Creation
 *
 *
 */
metadata {
	definition (name: "Child Battery Sensor (Sonar)", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Sensor"
        capability "Battery"

//        attribute "sonar", "String"

        command "update"
	}

	tiles(scale: 2) {
		multiAttributeTile(name: "battery", type: "generic", width: 6, height: 4, canChangeIcon: true) {
			tileAttribute("device.battery", key: "PRIMARY_CONTROL") {
				attributeState("battery", label: '${currentValue}%', unit:'${unit}', defaultState: true,
                backgroundColors: [
                                // Celsius
                                [value: 100, color: "#44b621"],
                                [value: 66, color: "#f1d801"],
                                [value: 33, color: "#d04e00"],
                                [value: 0, color: "#bc2323"]
						])

			}

		}
		standardTile("refresh", "device.refresh", inactiveLabel: false, decoration: "flat", width: 2, height: 2) {
			state "default", label:'Refresh', action: "update", icon: "st.secondary.refresh-icon"
		}

	}
       preferences {
       input "polling", "number", title: "Polling Frequency",
              description: "Set frequency of depth polling (seconds).", defaultValue: "60",
              required: false, displayDuringSetup: true

       input "minRead", "number", title: "Min Depth",
              description: "Set Minimum distance to register 100% full.", defaultValue: "15",
              required: false, displayDuringSetup: true

       input "tankDepth", "number", title: "Tank Depth",
              description: "Set Depth of Tank to register 0% full.", defaultValue: "40",
              required: false, displayDuringSetup: true

    }

}
void update() {
	parent.childSonar(device.deviceNetworkId, polling, minRead, tankDepth)
}
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
 *  on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License
 *  for the specific language governing permissions and limitations under the License.
 *
 *  Change History:
 *
 *    Date        Who            What
 *    ----        ---            ----
 *    2017-04-19  Dan Ogorchock  Original Creation
 *
 *
 */
metadata {
	definition (name: "Child Battery Sensor (Sonar)", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Sensor"
        capability "Battery"

//        attribute "sonar", "String"

        command "update"
	}

	tiles(scale: 2) {
		multiAttributeTile(name: "battery", type: "generic", width: 6, height: 4, canChangeIcon: true) {
			tileAttribute("device.battery", key: "PRIMARY_CONTROL") {
				attributeState("battery", label: '${currentValue}%', unit:'${unit}', defaultState: true,
                backgroundColors: [
                                // Celsius
                                [value: 100, color: "#44b621"],
                                [value: 66, color: "#f1d801"],
                                [value: 33, color: "#d04e00"],
                                [value: 0, color: "#bc2323"]
						])

			}

		}
		standardTile("refresh", "device.refresh", inactiveLabel: false, decoration: "flat", width: 2, height: 2) {
			state "default", label:'Refresh', action: "update", icon: "st.secondary.refresh-icon"
		}

	}
       preferences {
       input "polling", "number", title: "Polling Frequency",
              description: "Set frequency of depth polling (seconds).", defaultValue: "60",
              required: false, displayDuringSetup: true

       input "minRead", "number", title: "Min Depth",
              description: "Set Minimum distance to register 100% full.", defaultValue: "15",
              required: false, displayDuringSetup: true

       input "tankDepth", "number", title: "Tank Depth",
              description: "Set Depth of Tank to register 0% full.", defaultValue: "40",
              required: false, displayDuringSetup: true

    }

}
void update() {
	parent.childSonar(device.deviceNetworkId, polling, minRead, tankDepth)
}
