{#
    This is a Jinja template, everything in {{ }} or {% %} brackets is replaced with C code.
    Code inside brackets are Python snippets.

    Pro tip: dont use tabs in C code, replace them with four spaces (tabs cause weird formatting issues). 
-#}


{% import EMBEDDED_BASE_PATH + "/cangen/templates/macros.j2" as macros -%}


{% for msg in can_msgs %}
void receive_{{ macros.function_name(msg) }} (can_msg_t message) {
    // TODO
}
{% endfor %}