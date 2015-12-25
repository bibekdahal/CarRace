

function Update(dt)
	vehicle = self.btVehicle
	steeringIncrement = 0.15 * dt
    steeringClamp = 0.05

	if vehicle.speed > 90 then 
		engineForce = 0
	else
		if (Window.CheckKey(string.byte('W'))) then
			engineForce = 900
		elseif (Window.CheckKey(string.byte('S'))) then
			engineForce = -900
		else 
			engineForce = 0
		end
	end

    if (Window.CheckKey(string.byte('A'))) then
        steering = steering + steeringIncrement
        if (steering > steeringClamp) then
            steering = steeringClamp
		end
    end
    if (Window.CheckKey(string.byte('D'))) then
        steering = steering - steeringIncrement
        if (steering < -steeringClamp) then
            steering = -steeringClamp
		end
	end

    if (Window.CheckKey(string.byte(' '))) then 
		brake = 100
    else 
		brake = 0 
	end
        
    vehicle:ApplyEngineForce(2, engineForce);
    vehicle:SetBrake(2, brake);
    vehicle:ApplyEngineForce(3, engineForce);
    vehicle:SetBrake(3, brake);

    vehicle:SetSteering(0, steering);
    vehicle:SetSteering(1, steering);
end

engineForce = 0
brake = 0
steering = 0