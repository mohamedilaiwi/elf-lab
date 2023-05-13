#include "printing.h"
#include "elf_runner.h"

/*********************************************
    #define EI_NIDENT 16

    typedef struct {
        unsigned char e_ident[EI_NIDENT];
        uint16_t      e_type;
        uint16_t      e_machine;
        uint32_t      e_version;
        ElfN_Addr     e_entry;
        ElfN_Off      e_phoff;
        ElfN_Off      e_shoff;
        uint32_t      e_flags;
        uint16_t      e_ehsize;
        uint16_t      e_phentsize;
        uint16_t      e_phnum;
        uint16_t      e_shentsize;
        uint16_t      e_shnum;
        uint16_t      e_shstrndx;
    } ElfN_Ehdr;


    typedef struct {
        uint32_t   p_type;
        uint32_t   p_flags;
        Elf64_Off  p_offset;
        Elf64_Addr p_vaddr;
        Elf64_Addr p_paddr;
        uint64_t   p_filesz;
        uint64_t   p_memsz;
        uint64_t   p_align;
    } Elf64_Phdr;


**********************************************/
Elf64_Ehdr *elf  = NULL;     // Will contain information about an executable file's ELF Header
Elf64_Phdr *phdr = NULL;     // Will contain information about an executable file's Program Header
Elf64_Shdr *shdr = NULL;     // Will contain information of file's section header table
char *strtab = NULL;    // The string table
char *symstrtable = NULL;
FILE *fp = NULL;

char *STRING_INFO[] = {"NOTYPE", "OBJECT", "FUNC", "SECTION",
        "FILE", "LOPROC", "LOCAL", "GLOBAL", "WEAK", "LOPROC"};
char *BIND_TYPES[] = {"Local", "Global", "Weak"};
char *VISIBILITY[] = {"DEFAULT", "INTERNAL", "HIDDEN", "PROTECTED"};
/*
*    Returns true if Executable contains ELF header
*    Precondition: Elf is not null
*/
bool is_valid_elf() {
    unsigned char *e_ident = elf->e_ident;
    return e_ident[EI_MAG0] == 0x7F && e_ident[EI_MAG1] == 'E' && 
        e_ident[EI_MAG2] == 'L' && e_ident[EI_MAG3] == 'F';
}


/*
*   A executable or shared file's program header
*   Precondition: This file is executable or shared
*/
void create_program_header() {
    Elf64_Ehdr *e_program_Table = (Elf64_Ehdr *)((char *)elf + elf->e_phoff);
    phdr = malloc(elf->e_phentsize * elf->e_phnum);

    for (int i = 0; i < elf->e_phnum; i++) {
        memcpy((Elf64_Phdr *)((char *)phdr + (i * elf->e_phentsize)), e_program_Table, elf->e_phentsize);
        e_program_Table = (Elf64_Ehdr *)((char *)e_program_Table + elf->e_phentsize);
    }
}


Elf64_Shdr *create_section_header(FILE *fp, uint32_t offset) {
    Elf64_Shdr *section_header = malloc(elf->e_shentsize);
    if (fseek(fp, offset, SEEK_SET) == -1 || 
            fread(&section_header, sizeof(Elf64_Ehdr), 1, fp) == -1) {
        perror("create_section_header");
        return NULL;
    }
    return section_header;
}

/*
*   Creates the section header table
*   Precondition: Elf is not null
*/
void create_section_header_table() {
    size_t table_size = elf->e_shnum * elf->e_shentsize;
    Elf64_Shdr *table = malloc(table_size);
    
    for (int i = 0; i < elf->e_shnum; i++) {
        Elf64_Off offset = (Elf64_Off) ((char *)elf->e_shoff + (i * elf->e_shentsize));

        if (fseek(fp, offset, SEEK_SET) == -1 || 
                fread(&table[i], sizeof(Elf64_Shdr), 1, fp) == -1) {
            perror("Create Section Header Table");
        }
    }
    shdr = table;
}

/*
*   Uses the Section Header Table (SHT) to get the symbol name of each section
*   Precondition: SHT is already filled out
*/
char *get_symbol_name(const char *str_tab) {
    for (int i = 0; i < elf->e_shnum; i++) {
        Elf64_Shdr *temp_section = &shdr[i];

        if (temp_section -> sh_type == SHT_SYMTAB) { // exists in the symbol table
            int num_entries = temp_section->sh_size/temp_section->sh_entsize;
            Elf64_Off sym_offset = temp_section->sh_offset;
            Elf64_Sym symtab[num_entries * sizeof(Elf64_Sym)];
            

            if (fseek(fp, sym_offset, SEEK_SET) == -1 
                || fread(&symtab, num_entries, sizeof(Elf64_Sym), fp) == -1) 
                // perror("Failed to read symtable");

            wprintw(stdscr, "%4s: %18s%8s%7s%7s%7s %4s %s\n",
                "Num", "Value", "Size", "Type", "Bind", "Vis", "Ndx", "Name");
            wprintw(stdscr, "%4s: %18s%8s%7s%7s%7s %4s %s\n",
            "Num", "Value", "Size", "Type", "Bind", "Vis", "Ndx", "Name");
            wprintw(stdscr, "-------------------------------------------------------\n");   
            for (int j = 0; j < LINES - 1; j++) {
                Elf64_Sym *sym = &symtab[j];

                char *symbol_name = &symstrtable[sym->st_name];
                char *ndx = deal_with_NDX(symtab[j].st_shndx);
                wprintw(stdscr, "%4i: %018lx%8ld %7s%7s %7s %7s %s\n", j, sym->st_value, sym->st_size,
                        STRING_INFO[sym->st_info & 0xf], BIND_TYPES[ELF64_ST_BIND(sym->st_info)], 
                        VISIBILITY[sym->st_other], ndx, symbol_name);
                free(ndx);
            }
            // call function here
            deal_with_scroll(num_entries, symtab);
        }
    }
    wprintw(stdscr, "Press any key to exit.");
    return NULL;

}

void create_string_table() {
    Elf64_Shdr *strtab_hdr = &shdr[elf->e_shstrndx];
    strtab = malloc(strtab_hdr->sh_size);
    if (fseek(fp, strtab_hdr->sh_offset, SEEK_SET) == -1 || 
            fread(strtab, strtab_hdr->sh_size, 1, fp) == -1) 
            perror("Failed to read for String Table");
}

void create_symbol_string_table() {
    Elf64_Shdr* symstrtab_hdr;
    for (int i = 0; i < elf->e_shnum; i++) {
        if (shdr[i].sh_type == SHT_STRTAB && strcmp(".strtab", &strtab[shdr[i].sh_name]) == 0) {
            symstrtab_hdr = &shdr[i];
            break;
        }
    }
    if (!symstrtab_hdr) {
        fprintf(stderr, "Failed to find symbol string table section\n");
        exit(1);
    }

    // printf("Size: %li, offset: %li\n", symstrtab_hdr->sh_size, symstrtab_hdr->sh_offset);
    char* symstrtab = malloc(symstrtab_hdr->sh_size);
    if (fseek(fp, symstrtab_hdr->sh_offset, SEEK_SET) == -1 ||
            fread(symstrtab, symstrtab_hdr->sh_size, sizeof(char *), fp) == -1) {
        exit(1);
    }
    symstrtable = symstrtab;
}


void create_elf(char *file_name) {
    if (file_name == NULL) return;
    Elf64_Ehdr elf_buff; // temporary buffer for Elf64

    elf = malloc(sizeof(Elf64_Ehdr));       // allocate enough to store temp buff

/*     if (argc != 2) {
        fprintf(stderr, "Invalid `ber of arguments. Exiting...\n");
        goto cleanup;
    } */
    if ((fp = fopen(file_name, "rb")) == NULL) {
        fprintf(stderr, "Problem occured while opening from file. Exiting...\n");
        exit(1);
    }

    if (!fread(&elf_buff, sizeof(elf_buff), 1, fp)) {   // reading into elf_buff
        fprintf(stderr, "Problem occured while reading from file at %s: %i", __FILE__, __LINE__);
        exit(1);
    }
    *elf = elf_buff; // soft copy
    if (!is_valid_elf()) { // check if valid ELF
        fprintf(stderr, "Executable did not contain a valid elf header. Exiting...\n");
        exit(1);
    }

    // display_elf_header();
    if ((elf->e_type) == ET_EXEC || (elf->e_type) == ET_DYN) {
        create_program_header();
    }
    create_section_header_table();

    // creating the string table
    create_string_table();
    create_symbol_string_table();
    // get_symbol_name(strtab);

}


/* int main(int argc, char *argv[]) {
    create_elf(argv[1]);
    return 0;
} */
