
int strcmp(char *s, char *t){
	int i;
	for(i = 0;s[i] != '\0';i++){
		if(s[i] != t[i]){
			return 0;
		}
	}
	for(i = 0;t[i] != '\0';i++){
		if(s[i] != t[i]){
			return 0;
		}
	}
	return 1;
}

int strlen(char *str){
	int count = 0;
	while(1){
		if(str[count] == '\0'){
			break;
		}
		count++;
	}
	return count;
}

void string_getNext(char *str, char *dst){
	int i;
	for(i = 0;str[i] != ' ' && str[i] != '\0';i++){
		dst[i] = str[i];
	}
	dst[i] = '\0';
	return;
}

void cut_string(char *str, char *dst, int start_point){
	char cut_string[1024];
	int i;
	for(i = 0;i < 1024 && str[i] != '\0';i++){
		cut_string[i] = str[i+start_point];
	}
	cut_string[i] = 0;
	return;
}

void strcpy(char *copied_str, char *str, int length){
	int i;
	for(i = 0;i < length;i++){
		copied_str[i] = str[i];
	}
	copied_str[i] = '\0';
	return;
}
