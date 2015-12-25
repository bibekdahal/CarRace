

function Update(dt)
	dist = 5.0
	if self.properties.InverseCamera then 
		dist = -6.0 
	end
	target = self.properties.Target.transform
	tt = target:GetPosition()
	rot0 = target:GetWorldTransform().mat3.vec0
	pos = vec3(tt.x + rot0.x*dist, tt.y + rot0.y*dist + 2.4, tt.z + rot0.z*dist)
	self.transform:SetWorldTransform(mat4.AffineInverse(mat4.LookAt(pos, tt, vec3(0,1,0))))
end
