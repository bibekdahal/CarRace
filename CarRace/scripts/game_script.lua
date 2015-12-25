follow_script = AddScript("scripts/follow_script.lua")
input_script = AddScript("scripts/input_script.lua")

min = vec3(0,0,0) max = vec3(0,0,0)
car_mdl = AddModelEx("car.dat", min, max, true, mat4(1))
car_extents = vec3((max.x-min.x)/2, (max.y-min.y)/2, (max.z-min.z)/2)
car_shp = AddBoxShape(car_extents)
--car_shp = AddShape("car.shp", 1)

car_mdl2 = DuplicateModel(car_mdl)
cmdl = GetModel(car_mdl2)
for i = 0, cmdl.numMeshes-1 do
	omat = GetDiffuseMaterial(cmdl:GetMaterial(i))
    if (omat.diffuse_color.x > omat.diffuse_color.y) then   -- check if red component is high
		matid = AddDiffuseMaterial()
		mat = GetDiffuseMaterial(matid)
		mat.texture = omat.texture
		mat.diffuse_color = vec4.Add(omat.diffuse_color, vec4(0.0, 1.0, 0.0, 0.0));
		cmdl:SetMaterial(i, matid)
	end
end


wmodel = AddModelEx("wheel.dat", nil, nil, false, mat4.Rotate(-90, vec3(0,1,0)))
track_mdl = AddModel("race-track.dat")
track_shp = AddShape("race-track.shp", 100)

wheeldir = vec3(0,-1,0)   axle=vec3(0,0,-1)  wradius=0.5   wwidth=1   ch = -car_extents.y+0.5

entity0 = {
		Transform={Position=vec3(1.5,-2,-1), Rotation = quat.Rotate(-45, vec3(0,1,0))},
		Model={Model=car_mdl2},
		BTCollider={Shape=car_shp},
		BTRigidBody={Mass=800},
		BTVehicle={Wheels={
				{Construct={vec3(-car_extents.x+1.5, ch, -car_extents.z+0.3*wwidth), wheeldir, axle, 0.2, wradius, true}},
				{Construct={vec3(-car_extents.x+1.5, ch, car_extents.z-0.3*wwidth), wheeldir, axle, 0.2, wradius, true}, Reflect=true},
				{Construct={vec3(car_extents.x-1, ch, -car_extents.z+0.3*wwidth), wheeldir, axle, 0.2, wradius, false}},
				{Construct={vec3(car_extents.x-1, ch, car_extents.z-0.3*wwidth), wheeldir, axle, 0.2, wradius, false}, Reflect=true}
		}},
		Scripts = {input_script}
}

we0 = { Transform={Parent=entity0.Transform}, Model={Model=wmodel} }
we1 = { Transform={Parent=entity0.Transform}, Model={Model=wmodel} }
we2 = { Transform={Parent=entity0.Transform}, Model={Model=wmodel} }
we3 = { Transform={Parent=entity0.Transform}, Model={Model=wmodel} }

entity0.BTVehicle.Wheels[1].Entity = we0
entity0.BTVehicle.Wheels[2].Entity = we1
entity0.BTVehicle.Wheels[3].Entity = we2
entity0.BTVehicle.Wheels[4].Entity = we3



race_track = {
		Transform={Position=vec3(-30, -22, -6), Rotation = quat.Multiply(quat.Rotate(-100, vec3(0,1,0)), quat.Rotate(-90, vec3(1,0,0)))},
		Model={Model=track_mdl, Scale=100},
		BTCollider={Shape=track_shp},
		BTRigidBody={Mass=0.0}
}

camera1 = {
		Transform={},
		Camera={Viewport=Rect(0,0.5,1,0.5)},
		Scripts = {follow_script},
		Properties = {Target=entity0, InverseCamera=false}
}

camera2 = {
		Transform={},
		Camera={Viewport=Rect(0,0,1,0.5)},
		Scripts = {follow_script},
		Properties = {Target=entity0, InverseCamera=true}
}

amb = vec3(0.04,0.04,0.04)
light0 = {
	DirectionalLight={Ambient=amb, Color=vec3(1.2,1.2,1.2), Direction=vec3(-1.0,0.0,-1.0)}
}
light1 = {
	Transform={Position=vec3(0.6,0.0,0.0)}, 
	PointLight={Ambient=amb, Color=vec3(0.6,0.0,0.0), ConstantAttenuation=0.0001, LinearAttenuation=0.001, QuadraticAttenuation=0.02}
}
light2 = {
	Transform={Position=vec3(-19,-1.5,-30), Rotation=quat.Rotate(120, vec3(0,1,0))}, 
	SpotLight={Ambient=amb, Color=vec3(0.0,0.0,1.0), CutOffAngle=40.0, ConstantAttenuation=0.001, LinearAttenuation=0.001, QuadraticAttenuation=0.02}
}


test_mesh = AddBoxMesh(vec3(1,2,3))
test_matid = AddDiffuseMaterial()
test_mat = GetDiffuseMaterial(test_matid)
test_mat.diffuse_color = vec4(0.0, 1.0, 0.0, 1.0); 
test_entity = {
	Transform={Position=vec3(1.2,0.0,-12)},
	Mesh = {Mesh=test_mesh, Material=test_matid}
}

scene1 = {
	Entities = {
		entity0, race_track, camera1, camera2,
		we0, we1, we2, we3, light0, light1, light2, test_entity
	}
}

len = #scene1.Entities
num = 3
j=len+1
for i=len+1, len+num do
	entity1 = {
			Transform={Position=vec3(-20,-2,6*(i-len-1-7)), Rotation = quat.Rotate(-45, vec3(0,1,0))},
			Model={Model=car_mdl},
			BTCollider={Shape=car_shp},
			BTRigidBody={Mass=800},
			BTVehicle={Wheels={
					{Construct={vec3(-car_extents.x+1.5, ch, -car_extents.z+0.3*wwidth), wheeldir, axle, 0.2, wradius, true}},
					{Construct={vec3(-car_extents.x+1.5, ch, car_extents.z-0.3*wwidth), wheeldir, axle, 0.2, wradius, true}, Reflect=true},
					{Construct={vec3(car_extents.x-1, ch, -car_extents.z+0.3*wwidth), wheeldir, axle, 0.2, wradius, false}},
					{Construct={vec3(car_extents.x-1, ch, car_extents.z-0.3*wwidth), wheeldir, axle, 0.2, wradius, false}, Reflect=true}
			}},
			--Scripts = {{id=input_script}}
	}

	we10 = { Transform={Parent=entity1.Transform}, Model={Model=wmodel} }
	we11 = { Transform={Parent=entity1.Transform}, Model={Model=wmodel} }
	we12 = { Transform={Parent=entity1.Transform}, Model={Model=wmodel} }
	we13 = { Transform={Parent=entity1.Transform}, Model={Model=wmodel} }

	entity1.BTVehicle.Wheels[1].Entity = we10
	entity1.BTVehicle.Wheels[2].Entity = we11
	entity1.BTVehicle.Wheels[3].Entity = we12
	entity1.BTVehicle.Wheels[4].Entity = we13

	scene1.Entities[j]=entity1  j=j+1
	scene1.Entities[j]=we10     j=j+1
	scene1.Entities[j]=we11     j=j+1
	scene1.Entities[j]=we12     j=j+1
	scene1.Entities[j]=we13     j=j+1
end



Scenes  = { scene1 }
StartScene = scene1
