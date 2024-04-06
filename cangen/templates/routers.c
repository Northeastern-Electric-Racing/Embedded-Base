{#
    This is a Jinja template, everything in {{ }} or {% %} brackets is replaced with C code.
    Code inside brackets are Python snippets.

    Pro tip: dont use tabs in C code, replace them with four spaces (tabs cause weird formatting issues). 
-#}


{% import "macros.j2" as macros -%}


{#-
    This comment is for spacing purposes, the number of newlines after the closing bracket will
    be the number of newlines added by Jinja to the top of the template.
#}

switch (message.id) {
    {%- for msg in can_msgs %}
    case {{ msg.id }}:
        receive_{{ macros.function_name(msg) }}(message);
        break;
    {%- endfor %}
    default:
        // TODO: err
        break;
}