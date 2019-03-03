## Работа с файлове - Седмица 2

**Зад 1. 02-а-0100** 
	Направете копие на файла **/etc/passwd** във вашата home директория под името **my_passwd**.

<u>Примерно решение:</u>
	`cp /etc/passwd ~/my_passwd`
	The cp command is used to copy files and directories. The copies become independent of the originals (i.e., a subsequent change in one will not affect the other).

**Зад 2. 02-а-0600**
	Направете директория **practice-test** в home директорията ви. 
	Вътре направете директория **test1**. Можете ли да направите тези две неща наведнъж? Разгледайте нужната man страница.
	След това създайте празен файл вътре, който да се казва **test.txt**, преместете го в practice-test чрез релативни пътища.

<u>Примерно решение:</u>
	`mkdir -p practice_test/test1`
	`touch test.txt`
	`mv test.txt practice_test`

​	mkdir - The mkdir command is is used to create new directories. The `-p` switch create parents directories
​	touch - The touch command is the easiest way to create new, empty files. It is also used to change the timestamps (i.e., dates and times of the most recent access and modification) on existing files and directories.
​	mv - The mv command is used to rename and move files and directories.

**Зад 3. 02-a-0600**
	Копирайте файловете **f1, f2, f3** от директорията **/tmp/os2018/practice/01/** в директория **dir1**, намираща се във вашата home директория. Ако нямате такава, създайте я.

<u>*Примерно решение:*</u>
	`mkdir -p  /tmp/os2018/practice/01`
	``cd /tmp/os2018/practice/01`
	`touch f1 f2 f3`
	`mkdir dir1`
	`cp /tmp/os2018/practice/01/{f1,f2,f3} ~/dir1`

**Зад 4. 02-a-0601**
	Нека файлът **f2** бъде преместен в директория **dir2**, намираща се във вашата **home** директория и бъде преименуван на **numbers**.

<u>Примерно решение:</u>
	``mkdir ~/dir2 && cp ~/dir1/f2 ~/dir2/numbers`

**Зад 5. a-1200**
	Отпечатайте имената на **всички директории** в директорията **/home**.

<u>Примерно решение:</u>
	`cd /home && ls -d */`
	или
	`cd /home && ls -al | grep '^d'`

**Зад 6.** COMING SOON stay tuned

