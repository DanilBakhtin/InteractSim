# Версия
Unreal Engine 5.5.4

# Управление
- WASD - передвижение
- E - взаимодействие
- Q - показать/скрыть курсор мыши

# Ссылки
- [Описание](https://docs.google.com/document/d/1IbnPaxUEzTso-ZRoLhm0oE1LNnQm3aNKJEnPLvQn8HE/edit?usp=sharing)
- [Билд](https://drive.google.com/file/d/1nb3KNNU7Y-6CtSmM4Vg2JWbIizFC8o7U/view?usp=sharing) 

# Видео
<video width="320" height="240" controls>
  <source src="https://youtu.be/krs_aaHtaIc" type="video/mp4">
</video>

[![Видео](https://img.youtube.com/vi/krs_aaHtaIc/0.jpg)](https://youtu.be/krs_aaHtaIc)


# Где хранится файл, с информацией об объектах для билда?
- ...\AppData\Local\InteractSim\Saved\ObjectsData.txt

# Пример данных:
{
	"objects": [
		{
			"id": 1,
			"name": "Box1",
			"position":
			{
				"x": 1150,
				"y": 2050,
				"z": 200
			},
			"color": "red",
			"bIsActive": true,
			"objectType": "Box"
		},
		{
			"id": 2,
			"name": "Sphere2",
			"position":
			{
				"x": 550,
				"y": 2050,
				"z": 200
			},
			"color": "blue",
			"bIsActive": true,
			"objectType": "Sphere"
		},
		{
			"id": 3,
			"name": "Sphere3",
			"position":
			{
				"x": 1150,
				"y": 2050,
				"z": 0
			},
			"color": "blue",
			"bIsActive": false,
			"objectType": "Sphere"
		}
	]
}
