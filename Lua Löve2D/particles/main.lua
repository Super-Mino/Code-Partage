--Projet réalisé le 07/07/23

--Remerciements à David de Gamecodeur pour l'enseignement du principe des particules.

love.graphics.setDefaultFilter("nearest")
print(love.getVersion())

--[[
Les différentes fonctions sont plus ou moins répétitives, le principe reste le même :
une position de départ, un vecteur de déplacement, 
(facultatif : une taille et couleur particulières), pour simplement appliquer le vecteur
chaque frame jusqu'à la durée de vie qu'on a attribué à la particule. 
C'est l'organisation des différents paramètres qui donne son aspect à un groupe de particules. 
]]


local Particles = {}
dt = 0
total_particles_now = 0

--Les variables 'my_datas' font référence aux données concernées par la fonction dans laquelle elles se trouvent.

Particles.fade_circle = {
	current_time=0,
	max_time=2,
	current_color={1,1,1,1},
	way=1 --Détermine si "s'éteint" ici avec 1, ou "s'allume" avec -1.
}

function DrawFadeCircle(x, y, radius, datas, change_way)
	
	local my_datas = Particles.fade_circle
	if datas ~= nil then my_datas = datas end

	love.graphics.setColor(my_datas.current_color)
	love.graphics.circle("fill", x, y, radius)

	--Le canal alpha est déterminé par le pourcentage de la progression dans le temps total de la particule. Lors de la "disparition" le calcul donne abs(% -1). 
	--On enlève 1 pour avoir le % restant (le % progresse de 0 vers 1 or nous voulons 1 au 0%, 0.81 au 0.19% et ainsi de suite) mais est négatif, donc on le corrige avec abs.
	local alpha = math.abs(my_datas.current_time / my_datas.max_time - (my_datas.way+1)/2) -- (x+1)/2 permet de renvoyer 1 si x = 1, 0 si x = -1.
	my_datas.current_color[4] = alpha

	if my_datas.current_time >= my_datas.max_time then
		my_datas.current_time = 0
		if change_way then my_datas.way = my_datas.way*-1 end
	else
		my_datas.current_time = my_datas.current_time +dt
	end
end



Particles.basic_explosion = {
	list_of_particles={},
	begin_x=230,	--Modifiables plus loin dans le code à l'aide d'un clic droit à l'endroit souhaité.
	begin_y=100,
	mean_speed=0,
	dev_speed=0.35, --Ceux ci sont pour x comme pour y.
	mean_time=3.0,
	dev_time=2.8,
	mean_radius=2,
	dev_radius=1 -- 'dev' pour 'deviation', qui ici veut dire (dans le contexte de la distribution normal) d'environ combien on peut se décaler de notre valeur moyenne de base (en dessous tel qu'au dessus).
}

function BasicExplosion(n, gravity)
	local my_datas = Particles.basic_explosion

	if #my_datas.list_of_particles == 0 then
		for i=1, n do
			local particle = {
				x = my_datas.begin_x,
				y = my_datas.begin_y,
				radius = love.math.randomNormal(my_datas.dev_radius, my_datas.mean_radius),
				x_speed = love.math.randomNormal(my_datas.dev_speed, my_datas.mean_speed),
				y_speed = love.math.randomNormal(my_datas.dev_speed, my_datas.mean_speed),
				help_datas = { 
					current_time = 0,
					max_time = love.math.random(my_datas.dev_time, my_datas.mean_time),
			  		current_color = {
						math.abs(love.math.randomNormal(1, 0.5)),
						math.abs(love.math.randomNormal(1, 0.5)),
						math.abs(love.math.randomNormal(1, 0.5)),
						1
					},
					way = 1
				}
			}

			if particle.x == 0 and particle.y == 0 then particle.y = 0.1 end

			table.insert(my_datas.list_of_particles, particle)
		end
	
	else
		for index = #my_datas.list_of_particles, 1, -1 do

			local particle = my_datas.list_of_particles[index] 

			DrawFadeCircle(particle.x, particle.y, particle.radius, particle.help_datas, false)

			particle.x = particle.x +particle.x_speed
			particle.y = particle.y +particle.y_speed 
			particle.y_speed = particle.y_speed +gravity

			if particle.help_datas.current_time > particle.help_datas.max_time then
				table.remove(my_datas.list_of_particles, index)
			end
		end
	end

end



Particles.falling_particles = {
	list_of_particles = {},
	mean_fall_speed = 0.4,
	dev_fall_speed = 0.2,
	mean_radius = 3,
	dev_radius = 1.19,
	mean_time = 3,
	dev_time = 2,
	dev_x = 3,
	dev_y = 2
}

function AddFallingParticles(x, y, in_green)
	local my_datas = Particles.falling_particles 

	local particle = {
		x = love.math.randomNormal(my_datas.dev_x, x),
		y = love.math.randomNormal(my_datas.dev_y, y),
		radius = love.math.randomNormal(my_datas.dev_radius, my_datas.mean_radius),
		x_speed = 0,
		y_speed = math.abs(love.math.randomNormal(my_datas.dev_fall_speed, my_datas.mean_fall_speed)),
		help_datas = { 
			current_time = 0,
			max_time = love.math.random(my_datas.dev_time, my_datas.mean_time),
			current_color = {
				math.abs(love.math.randomNormal(1, 0.5)),
				0,
				0, 
				1
			},
			way = 1
		}
	}

	if in_green then 
		particle.help_datas.current_color[2] = particle.help_datas.current_color[1]
		particle.help_datas.current_color[1] = 0
	end

	table.insert(my_datas.list_of_particles, particle)
end

function FallingParticles()
	local my_datas = Particles.falling_particles 

	for index = #my_datas.list_of_particles, 1, -1 do
		local particle = my_datas.list_of_particles[index]
		
		DrawFadeCircle(particle.x, particle.y, particle.radius, particle.help_datas, false)

		particle.y = particle.y +particle.y_speed

		if particle.help_datas.current_time > particle.help_datas.max_time then
			table.remove(my_datas.list_of_particles, index)
		end
	end

end



Particles.snow = {
	list_of_particles = {},
	mean_x_speed = 0.05,
	dev_x_speed = 0.2,
	mean_y_speed = 0.3,
	dev_y_speed = 0.15,
	mean_radius = 2,
	dev_radius = 1.19,
	mean_time = 20,
	dev_time = 4,
	area_size = 250,
	area_x = 0,
	area_y = 0,
	max = 400
}

function Snow()
	local my_datas = Particles.snow 
	my_datas.area_x = love.graphics.getWidth()-my_datas.area_size
	my_datas.area_y = love.graphics.getHeight()-my_datas.area_size

	local to_add_count = 0

	if #my_datas.list_of_particles < my_datas.max then
		to_add_count = my_datas.max - #my_datas.list_of_particles
	end

	for i=1, to_add_count do
		local particle = {
			x = my_datas.area_x + math.abs(love.math.randomNormal(my_datas.area_size, 0)),
			y = my_datas.area_y + math.abs(love.math.randomNormal(my_datas.area_size/8, 0)),
			radius = love.math.randomNormal(my_datas.dev_radius, my_datas.mean_radius),
			x_speed = love.math.randomNormal(my_datas.dev_x_speed, my_datas.mean_x_speed),
			y_speed = love.math.randomNormal(my_datas.dev_y_speed, my_datas.mean_y_speed),
			help_datas = { 
				current_time = 0,
				max_time = love.math.random(my_datas.dev_time, my_datas.mean_time),
		  		current_color = {1,1,1,1},
				way = 1
			}
		}

		if particle.x == 0 and particle.y == 0 then particle.y = 0.1 end
		table.insert(my_datas.list_of_particles, particle)
	end

	for index = #my_datas.list_of_particles, 1, -1 do
		local particle = my_datas.list_of_particles[index]
		
		DrawFadeCircle(particle.x, particle.y, particle.radius, particle.help_datas, false)

		particle.x = particle.x +particle.x_speed
		particle.y = particle.y +particle.y_speed

		if particle.help_datas.current_time > particle.help_datas.max_time 
			or particle.x < my_datas.area_x  
			or particle.x > my_datas.area_x + my_datas.area_size
			or particle.y < my_datas.area_y  
			or particle.y > my_datas.area_y + my_datas.area_size
		then
			table.remove(my_datas.list_of_particles, index)
		end
	end

end



Particles.smoke = {
	list_of_particles = {},
	mean_x_speed = 0.03,
	dev_x_speed = 0.01,
	mean_y_speed = -0.5,
	dev_y_speed = 0.05,
	mean_radius = 27,
	dev_radius = 10.15,
	mean_time = 20,
	dev_time = 2,
	area_size = 25,
	area_x = 50,
	area_y = 0,
	max = 35
}

function Smoke()
	local my_datas = Particles.smoke 
	my_datas.area_y = my_datas.area_size

	local to_add_count = 0

	if #my_datas.list_of_particles < my_datas.max then
		to_add_count = my_datas.max - #my_datas.list_of_particles
	end

	for i=1, to_add_count do
		local particle = {
			x = my_datas.area_x + math.abs(love.math.randomNormal(my_datas.area_size, 0)),
			y = love.graphics.getHeight() +my_datas.mean_radius*2,
			radius = love.math.randomNormal(my_datas.dev_radius, my_datas.mean_radius),
			x_speed = love.math.randomNormal(my_datas.dev_x_speed, my_datas.mean_x_speed),
			y_speed = love.math.randomNormal(my_datas.dev_y_speed, my_datas.mean_y_speed),
			help_datas = { 
				current_time = 0,
				max_time = love.math.random(my_datas.dev_time, my_datas.mean_time),
		  		current_color = {1,1,1,1},
				way = 1
			}
		}

		if particle.x == 0 and particle.y == 0 then particle.y = 0.1 end
		table.insert(my_datas.list_of_particles, particle)
	end

	for index = #my_datas.list_of_particles, 1, -1 do
		local particle = my_datas.list_of_particles[index]
		
		DrawFadeCircle(particle.x, particle.y, particle.radius, particle.help_datas, false)

		particle.x = particle.x +particle.x_speed
		particle.y = particle.y +particle.y_speed

		if particle.help_datas.current_time > particle.help_datas.max_time then
			table.remove(my_datas.list_of_particles, index)
		end
	end

end


--======================================================================================
function love.load()
	love.graphics.setBackgroundColor(0.2, 0.2, 0.2, 1)
end





--======================================================================================
function love.update(delta_time)
	dt = delta_time

	if love.mouse.isDown(1) then
		AddFallingParticles(love.mouse.getX(), love.mouse.getY(), true)
	else
		AddFallingParticles(love.mouse.getX(), love.mouse.getY())
	end

	if love.mouse.isDown(2) then
		Particles.basic_explosion.begin_x = love.mouse.getX()
		Particles.basic_explosion.begin_y = love.mouse.getY()
	end

	

	total_particles_now = 1 + -- 1 pour le fade circle qui est tout seul
		#Particles.falling_particles.list_of_particles +
		#Particles.basic_explosion.list_of_particles +
		#Particles.snow.list_of_particles +
		#Particles.smoke.list_of_particles

end






--======================================================================================
function love.draw()
	DrawFadeCircle(400, 30, 18, nil, true)
	BasicExplosion(700, 0.009)
	FallingParticles()
	Snow()
	Smoke()

	love.graphics.setColor(1, 1, 1, 1)
	love.graphics.print("FPS : "..love.timer.getFPS())
	love.graphics.print("total particles now : "..total_particles_now, 0, 20)
	
end 





--======================================================================================
function love.keypressed(key)
end

--======================================================================================
-- function love.mousepressed(x, y, button, istouch, presses)
-- 	-- if button == 1 then
-- 	-- 	AddFallingParticles(x, y)
-- 	-- end
-- end