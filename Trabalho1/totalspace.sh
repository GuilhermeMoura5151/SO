#!/bin/bash
#
 #Guilherme Moura

#tspace=0
#flags#
opa="0"
opr="0"
opn=".*"
opl="i"
opd="$(date -d "1970-01-01 00:00:00 UTC" +%s)"  #DATA INICIAL
opL="i"

#Verificação de argumentos
while getopts n:l:L:d:ra opt; do
	case "$opt" in
		n ) #Seleccao atraves do nome do ficheiro
            opn="$OPTARG"
            echo "-n: $OPTARG"
            
			;;
		l ) #Quantos entre os maiores de cada diretoria devem ser considerados
            if [[ "$opL" -ne "i" ]]; then
                echo "Não pode usar -l e -L ao mesmo tempo"
                exit 1
            fi
            if [[ "$OPTARG" =~ ^[0-9]+$ ]]; then
                opl="$OPTARG"
            else
                echo "Opção -l "$OPTARG" não é um número"
            fi
            echo "-l: $OPTARG"
			
			;;
		L ) #Quantos entre os maiores de TODAS as diretorias devem ser considerados
            if [[ "$opl" -ne "i" ]]; then
                echo "Não pode usar -l e -L ao mesmo tempo"
                exit 1
            fi
            if [[ "$OPTARG" =~ ^[0-9]+$ ]]; then
                opL="$OPTARG"
            else
                echo "Opção -L "$OPTARG" não é um número"
            fi
            echo "-L: $OPTARG"

			;;
		d ) #Especificacao da data maxima de acesso ao ficheiros
            test="$(date -d "$OPTARG" +%s &> /dev/null)"
            if [[ $? -eq 0 ]]
            then
                opd="$(date -d "$OPTARG" +%s)"
            else
                echo "Opção -d "$OPTARG" não é uma data"
            fi
            echo "-d: $OPTARG"

			;;
        r ) #Sort Menor ao maior
			echo "-r"
            opr=1

			;;
		a ) #Sort Maior ao menor
			echo "-a"
            opa=1

			;;
		* ) #default
			printf "Argumento Invalido!"
			exit 1

			;;
		
    esac
done

shift $(( $OPTIND - 1 ))

##############################################################

### funcao para -l
func_l()
{
    ## Alocação de variáveis locais
    local Ndir="$(ls -l "$1" | grep -c ^d )" # N.º de subdiretórios

    local Nfile="$(ls "$1" -ltu --time-style=+%s | sort -k6 | grep ^"$4"$ | awk -v adate="$3" '{if($6>adate) print;}'| grep -c ^-)" # N.º de ficheiros

    local dir_size  # Tamanho do diretório

    # Se o diretório não tiver ficheiros, o n.º de bytes associado ao diretório é 0, se tiver, saca a soma dos bytes dos l maiores ficheiros
    if [[ "$Nfile" -eq "0" ]]; then
        dir_size="0"
    else
        # verificar se o $2 tem parametro numérico
        local l_select
        if [[ "$2" -eq "i" ]]; then
            l_select="$Nfile"
        else
            l_select="$2"
        fi
        dir_size="$(ls "$1" -ltu --time-style=+%s | sort -k6 | grep ^"$4"$ | awk -v adate="$3" '{if($6>adate) print;}'| grep ^- | awk '{print '\$5'}' | sort -n | tail -"$l_select" | paste -sd+ | bc)"
    fi

    local rel_dir="$1" 

	size="$dir_size"
    echo ""$dir_size" "$rel_dir""

    return 0
}

##################################
### funcao para -L
func_L ()
{
    ## Alocação de variáveis locais
    local Ndir="$(ls -l "$1" | grep -c ^d)" # N.º de subdiretórios

    local Nfile="$(ls "$1" -ltu --time-style=+%s | sort -k6 | tail -n +2 | grep ^"$4"$ | awk -v adate="$3" '{if($6>adate) print;}'| grep -c ^-)" # N.º de ficheiros

    # verificar se o $2 tem parametro numérico
    local l_select
    if [[ "$2" -eq "i" ]]; then
        l_select="$Nfile"
    else
        l_select="$2"
    fi

    # Guardar os tamanhos dos ficheiros num array
    local rel_dir="$1"
    ls "$1" -ltu --time-style=+%s | sort -k6 | tail -n +2 | grep ^"$4"$ | grep ^- | awk -v adate="$3" '{if($6>adate) print;}' | awk -v var="$rel_dir" '{for(i=8;i<=NF;i++) $7=$7 OFS $i; print $5 " "var"/"$7}' | sort -k1n | tail -"$l_select"
}


###########################################################################################


if [[ "$opr" -eq "1" && "$opa" -eq "0" ]]; then
{
for dir in "$@"
do
    if ! [[ -d "$dir" ]]; then  # verificar directoria
        echo "$dir" não é uma directoria
        exit 1
    fi

    test="$(ls "$dir" &> /dev/null)"
	if [[ $? -ne 0 ]]; then
        echo "NA $dir"
    else
        if [[ "$opL" -eq "i" ]]; then    #-l e -L nao podem ser usadas em conjunto
            size="0"
            func_l "$dir" "$opl" "$opd" "$opn"
        else
            func_L "$dir" "$opL" "$opd" "$opn"
        fi
    fi
done;
} | sort -n 

elif [[ "$opr" -eq "0" && "$opa" -eq "0" ]]; then
{
for dir in "$@"
do
    if ! [[ -d "$dir" ]]; then  # verificar directoria
        echo "$dir" não é uma directoria
        exit 1
    fi

    test="$(ls "$dir" &> /dev/null)" # 2&>1 2 1>1 2>1
    if [[ $? -ne 0 ]]; then
        echo "NA $dir"
    else
        if [[ "$op_L" -eq "i" ]]; then    #-l e -L nao podem ser usadas em conjunto
            size="0"
            func_l "$dir" "$opl" "$opd" "$opn"
        else
            func_L "$dir" "$opL" "$opd" "$opn"
        fi
    fi
done;
} | sort -k1nr # sort por campo

elif [[ "$opr" -eq "0" && "$opa" -eq "1" ]]; then
{
for dir in "$@"   
do
    if ! [[ -d "$dir" ]]; then    # verificar directoria
        echo "$dir" não é uma directoria
        exit 1
    fi

    test="$(ls "$dir" &> /dev/null)"
    if [[ $? -ne 0 ]]; then
        echo "NA $dir"
    else
        if [[ "$opL" -eq "i" ]]; then    #-l e -L nao podem ser usadas em conjunto
            size="0"
            func_l "$dir" "$opl" "$opd" "$opn"
        else
            func_L "$dir" "$opL" "$opd" "$opn"
        fi
fi
done;
} | sort -k2  # sort por campo

else
{
for dir in "$@"
do
    if ! [[ -d "$dir" ]]; then  # verificar directoria
        echo "$dir" não é uma directoria
        exit 1
    fi

    test="$(ls "$dir" &> /dev/null)"
    if [[ $? -ne 0 ]]; then
        echo "NA $dir"
    else
        if [[ "$opL" -eq "i" ]]; then    #-l e -L nao podem ser usadas em conjunto
            size="0"
            func_l "$dir" "$opl" "$opd" "$opn"
        else
            func_L "$dir" "$opL" "$opd" "$opn"
        fi
fi
done;
} | sort -k2r   # sort por campo

fi
