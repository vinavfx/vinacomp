#include <write_node.h>

write_node::write_node()
{
    load_default_params("write");
}

write_node::~write_node() {}

void write_node::render(render_data *rdata, QJsonObject *params) {}
