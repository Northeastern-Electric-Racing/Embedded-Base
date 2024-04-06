{#
    This is a Jinja template, everything in {{ }} or {% %} brackets is replaced with C code.
    Code inside brackets are Python snippets.

    Pro tip: dont use tabs in C code, replace them with four spaces (tabs cause weird formatting issues). 
-#}


{% import "macros.j2" as macros -%}


{% for msg in can_msgs %}
can_msg_t* send_{{ macros.function_name(msg) }}
(
    can_t *can,
    {%- for field in msg.fields %}
    {{+ macros.field_type(field) }} {{ macros.field_name(field) }},
    {%- endfor %}
    uint8_t len
)
{
    struct __attribute__((packed)) {
		{%- for field in msg.fields %}
        {{+ macros.field_type(field) }} {{ macros.field_name(field) }} : {{ field.size }};	/* {{ field.unit }} */
		{%- endfor %}
    } msg_data;

	{% for field in msg.fields %}
    msg_data.{{ macros.field_name(field) }} = {{ macros.field_name(field) }};
    {%- endfor %}

    can_msg_t msg;
    msg.id = {{ msg.id }};
    msg.len = sizeof(msg_data);
    memcpy(msg.data, &msg_data, sizeof(msg_data));

    return &msg;
}
{% endfor %}