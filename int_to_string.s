.text
.global int_to_string


int_to_string:

    pushq %rbp
	movq %rsp, %rbp


    xorq %rdx, %rdx # Чищу регситр
    movq %rdi, %rax # Заношу число в регистр


    subq $1, %rsp   # Вычитаю из стека 8 бит под перенос строки 
    movq $10, 0(%rsp)   # Заношу код переноса строки  



    movq $10, %rbx # делитель
    movq $0, %rcx # Завожу счетчик len


loop:

    divq %rbx #  Разделить %rax == 123 na %rbx = 10
    subq $1, %rsp   #   Выделяю место на стеке под цифру
    movb %dl, 0(%rsp)  # Заношу цифру на выделенное место на стеке
    addb $48, 0(%rsp)  # Прибовляю число к ASCII символу для получения цифры
    xorq %rdx, %rdx #   Чищу регистр

    addq $1, %rcx   #  len += 1 

    cmpq $0, %rax # Условие выхода из цикла
    je loop_end # Если rax == 0 -> go to the loop_end 
    jmp loop # else начинаю цикл заново 


loop_end:


    addq $1, %rcx   # Добавляю

    movq $1, %rax   # Номер системной функции sys_write 
    movq $1, %rdi   # Куда выводить - на экран
    movq %rsp, %rsi # Адресс начала вывода символов
    movq %rcx, %rdx   # Количество выводимых символов 
    syscall


    movq %rbp, %rsp # Восстанавливаю уаазательна стек 
    popq %rbp

    ret
