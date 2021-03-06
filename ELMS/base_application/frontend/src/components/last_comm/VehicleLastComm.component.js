/*
* ELMS - Trevor Frame, Andrew Freitas, Deborah Kretzschmar
*
* This file contains the code for displaying the offline
* vehicles page.
*/

import React, { Component } from "react"
import Vehicle from "../vehicle_boilerplate/VehicleProp.js"
import VehicleBase from "../vehicle_boilerplate/VehicleBase"
import VehicleRow from "../vehicle_status/VehicleRow.component"


export default class OfflineVehicle extends VehicleBase{
    constructor(props){
        super(props);
        this.getOfflineVehicles = this.getOfflineVehicles.bind(this);
        this.getNearestVehicle = this.getNearestVehicle.bind(this);
        this.getActiveTime = this.getActiveTime.bind(this);
        this.state={vehicles: [], nearest_vehicle: 0}
        this.vehicleQuery = 'https://elms-base-application.uc.r.appspot.com/vehicles/offline'
    }

    //automatically get vehicle list and parse by status
    getOfflineVehicles(){
        if(this.state.vehicles.length > 0)
        {
            return this.state.vehicles.map(currentVehicle => {
                return <Vehicle vehicle={currentVehicle} getNearestVehicle={this.getNearestVehicle} getActiveTime={this.getActiveTime} key={currentVehicle._id}
                typeClass="active_time" typeText="Offline time: "/>;
            })
        }
    }

    render(){
        return(
            <div className="vehicle_status_tiles">
                <div className="offline_vehicles">
                    <hr className="offline_vehicles_line"/>
                        <p className="offline_vehicles_text">Offline</p>
                    <hr className="offline_vehicles_line"/>
                    { this.getOfflineVehicles() }
                </div>
                <VehicleRow />
            </div>
        )
    }
}

