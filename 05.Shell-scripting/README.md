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

```bash
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

````bash
#!/bin/bash

# Ask for user input
read -p "What is your name?": userInput

echo "Hello, $userInput"
````

**Зад 6. 05-b-2800**<br/>Да се напише shell скрипт, който проверява дали подаденият му параметър се състои само от букви и цифри.

````bash
#!/bin/bash
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

``````bash
#!/bin/bash
username=$(whoami)
if [[ $username =~ ^s[0-9]+$ ]]; then
    echo "${username:1}" # Cut first symbol
else
    echo "DANGER! It's a teacher!"
fi
``````

**Зад 8. 05-b-3100**<br/>Да се напише shell скрипт, който приканва потребителя да въведе низ - потребителско име на потребител от системата - след което извежда на стандартния изход колко пъти потребителят се е логнал.

``````bash
#!/bin/bash

# Task 05-b-3200
read -p "Enter username from the system:" username

# Check if user exists in the system
users_count=$(cat /etc/passwd | cut -d ':' -f 1 | egrep -c "^$username\$")
if [[ $users_count -ne 1 ]]; then
    echo "User ${username} does not exist in the system"
    exit 1
fi

# command who contains information for logged in users in the system
user_logged_count=$(echo "$(who)" | awk '{print $1}' | egrep -c "^$username\$")
if [[ $user_logged_count -eq 0 ]]; then
    echo "User ${username} is not logged in -> 0"
else
    echo "User ${userame} is logged in ${user_logged_count} times"
fi

exit 0
``````

**Зад 9. 05-b-3200**<br/>Да се напише shell скрипт, който приканва потребителя да въведе пълното име на директория и извежда на стандартния изход подходящо съобщение за броя на всички файлове и директории в нея.

````bash
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

```  bash
#!/bin/bash
read -p "Enter first file: " firstFile
read -p "Enter second file: " secondFile
read -p "Enter output file: " output_file

echo"Reading the content of first two files and saving it sorted in the third file..."
cat $firstFile $secondFile | sort > $output_file
echo "Result saved in ${output_file}"
```

**Зад 11. 05-b-3400**<br/>Да се напише shell скрипт, който чете от стандартния вход име на файл и символен низ, проверява дали низа се съдържа във файла и извежда на стандартния изход кода на завършване на командата.

```bash
#!/bin/bash

read -p "Enter text file: " input_file
read -p "String to search: " target_string

echo "Searching \"${target_string}\" in ${input_file}"
grep_exit_code=$(grep -q "${target_string}" ${input_file} ; echo $?)

echo "$grep_exit_code"
```

**Зад 12. 05-b-4200** <br/>Имате компилируем (a.k.a няма синтактични грешки) source file на езика C. Напишете shell script, който да покзва колко е дълбоко най-дълбокото nest-ване (влагане). По-лесно ли щеше да е за файл с Python код?

Примерен .c файл:

```c
int main(int argc, char *argv[]) {
	
	if (argc == 1) {
		printf("There is only 1 argument");
	} else {
		printf("There are more than 1 arguments");
	}
	
	return 0;
}
```

Тук влагането е 2, понеже имаме main блок, а вътре в него if блок.

Примерно извикване на скрипта: <br/> ./count_nesting sum_c_code.c

Изход: <br/>The deepest nesting is 2 levels

`````bash
#!/bin/bash

c_file=$1
if [[ ! -r ${c_file} ]]; then
    echo "Error! File ${c_file} is not readable!"
    exit 1
fi
if [ ${file:-2} == ".c" ]; then
    echo "Error! File ${c_file} has not .c extension!"
    exit 1
fi

# -i is stands for integer
declare -i max_nesting=0
declare -i nesting=0

# Read input symbol by symbol
while IFS= read -r -n 1 symbol; do
    if [[ $symbol == "}" ]]; then
        nesting=$((nesting-1))
    elif [[ $symbol == "{" ]]; then
        nesting=$((nesting+1))
    fi

    if (( max_nesting < nesting )); then
        max_nesting=$nesting
    fi
done < "$c_file"

echo  "The deepest nesting is ${max_nesting} levels"
`````

**Зад 13. 05-b-4300**<br/>Напишете shell script, който по подаден като аргумент файл с map между \<nickname\> -> <реален username в os-server> и nickname, ще ви улесни да изпращате съобщения на хората.

```
Пример за файл указател:
tinko	s61966
minko	s881234
ginko	s62000
dinko	s77777
senpai	velin
```

Примерно извикване на програмата:<br/>./send_message myAddressBook dinko

`````
// Work in progress
`````