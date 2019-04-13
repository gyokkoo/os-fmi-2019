## Shell скриптове

**Зад 1. 05-а-2000**<br/>Сменете вашия prompt с нещо по желание. После върнете оригиналния обратно.

`PS_OLD=$PS1`

`$PS1="enter command"`

`PS1=$PS_Old`

**Зад 2. 05-а-2100**<br/>Редактирайте вашия .bash_profile файл, за да ви поздравява (или да изпълнява някаква команда по ваш избор) всеки път, когато влезете в системата.

`vi ~/.bash_profile`  insert `echo "Welcome to unix, dear friend!"`

**Зад 3. 05-а-2200**<br/>Направете си ваш псевдоним (alias) на полезна команда.

`vi ~/bashrc` insert `alias c='clear'`

**Зад 4. 05-а-4000**<br/>Напишете shell script (който ще използвате от вашия линукс), който да приема параметър име на файл, който искате да копирате от os-server и да го копира в текущата директория (на вашия линукс).

Пример за употреба:

```
# сваля /etc/passwd от os-server на вашия линукс
./get_remote_file /etc/passwd
```

`vi ~/bin/get_remote_file`

```
#!/bin/bash

# get_remote_file
# This script copies a file from remote unix to local unix machine

# Check for an argument
if [[ ! $1  ]]; then
    echo "Missing filename argument"
    exit 1
fi

filename="$1"
destination="$PWD"

echo "Trying to get ${filename} from remote server"

scp "s62117@os-server:${filename}" "${destination}"
```

`chmod +x ~/bin/get_remote_file`

**Зад 5. 05-b-2000**<br/>Да се напише shell скрипт, който приканва потребителя да въведе име (низ) и изпечатва "Hello, името".

````
#!/bin/bash

# Ask for user input
read -p "What is your name?": userInput

echo "Hello, $userInput"
````

**Зад 6. 05-b-2800**<br/>Да се напише shell скрипт, който проверява дали подаденият му параметър се състои само от букви и цифри.

````
#!/bin/bash.
if [[ ! $1 ]]; then
    echo "Missing argument"
    exit 1
fi

if [[ ! $1 =~ [^a-zA-Z0-9]+ ]]; then
    echo "Your input contains only letters and digits!"
else
    echo "Your input does not contain only letter and digits!"
fi
````

**Зад 7. 05-b-3000**<br/>Да се напише shell скрипт, който извежда на стандартния изход факултетния номер на потребителя, който изпълнява скрипта, или казва "DANGER! It's a teacher!", ако потребителското име не е факултетен номер.

// TODO

**Зад 8. 05-b-3100**<br/>Да се напише shell скрипт, който приканва потребителя да въведе низ - потребителско име на потребител от системата - след което извежда на стандартния изход колко пъти потребителят се е логнал.

// TODO

**Зад 9. 05-b-3200**<br/>Да се напише shell скрипт, който приканва потребителя да въведе пълното име на директория и извежда на стандартния изход подходящо съобщение за броя на всички файлове и директории в нея.

````
#!/bin/bash
read -p "Enter directory: " dir

if [[ ! -d $dir ]]; then
    echo "${dir} is not a directory"
    exit 1
fi

files_count=$(find "$dir" -type f | wc -l)
directories_count=$(find "$dir" -type d | wc -l)
echo "${dir} has total ${files_count} files and ${directories_count} directories"
exit 0
````

**Зад 10. 05-b-3300**<br/>Да се напише shell скрипт, който чете от стандартния вход имената на 3 файла, обединява редовете на първите два, подрежда ги по азбучен ред и резултата записва в третия файл.

// TODO