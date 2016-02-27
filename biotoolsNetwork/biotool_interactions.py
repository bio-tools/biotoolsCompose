import json
from pprint import pprint


with open("bio.tools.json", encoding="utf8") as data_file:
    data = json.load(data_file)


idx2name = dict()      # Look-up the tool name by its index 
input2tool = dict()    # Look-up all the tools that uses a particular EDAM input format
output2tool = dict()   # Look-up all the tools that uses a particular EDAM output format 

# For all the tools:
for idx, tool in enumerate(data):
    # Find and store the tool name:
    name = tool['name']
    idx2name[idx] = name
    
    # Now go through the JSON format for this tool.
    # First loop through all the functions:
    if 'function' in tool:
        for function in tool['function']:
            # Now all the inputs of the tool function:
            if 'input' in function:
                for f_input in function['input']:
                    # Then all the data formats used for the input:
                    if 'dataFormat' in f_input:
                        for dataFormat in f_input['dataFormat']:
                            # Extract and keep the EDAM url:
                            if 'uri' in dataFormat:
                                EDAM_url = dataFormat['uri']
                                # Get the EDAM format id from the url:
                                EDAM_id = EDAM_url.split('/')[-1]
                                # Store the tool index for this EDAM input format:
                                if EDAM_id in input2tool:
                                    input2tool[EDAM_id].append(idx)
                                else:
                                    input2tool[EDAM_id] = [idx]
                            else: # No url defined
                                pass
                    else: # No dataFormat or EDAM_url defined
                        pass
            else: # No input defined
                pass

            # Same procedure as with the input defined above:
            if 'output' in function:
                for f_output in function['output']:
                    if 'dataFormat' in f_output:
                        for dataFormat in f_output['dataFormat']:
                            if 'uri' in dataFormat:
                                EDAM_url = dataFormat['uri']
                                EDAM_id = EDAM_url.split('/')[-1]
                                if EDAM_id in output2tool:
                                    output2tool[EDAM_id].append(idx)
                                else:
                                    output2tool[EDAM_id] = [idx]
                            else: # No url defined
                                pass
                    else: # No dataFormat defined
                        pass
            else: # No output defined
                pass
        
    else: # No function defined
        pass


# Remove duplicate entries:
for EDAM_id, outtool_idx in output2tool.items():
    output2tool[EDAM_id] = sorted(list(set(outtool_idx)))

for EDAM_id, intool_idx in input2tool.items():
    input2tool[EDAM_id] = sorted(list(set(intool_idx)))


# Find the tools with matching output and input:
out_match_in = dict()
# Look through all the EDAM output formats:
for EDAM_id, outtool_idx in output2tool.items():
    # Is the output format also existing as and input to another tool?:
    if EDAM_id in input2tool:
        intool_idx = input2tool[EDAM_id]
        # Now make the connection between the list of input tools and each matching output tool:
        for tool in outtool_idx:
            if tool in out_match_in:
                # Remove dublicates on the fly, or the dict will grow huge:
                out_match_in[tool] = sorted(list(set(out_match_in[tool] + intool_idx)))
            else:
                out_match_in[tool] = intool_idx
    else:
        pass


# Print the results to a Cytoscape tab separated interaction file:
with open("biotools.interactions", 'w') as file_handle:
    # Print header:
    print('{0}\t{1}\t{2}\t{3}'.format('name', 'source_node', 'interaction_type', 'target_node'), end="\n", file=file_handle)
    # Go through all tools with output matching an input:
    for out_tool, in_list in out_match_in.items():
        # Convert the output tool index to its name:
        out_name = idx2name[out_tool]
        # Print each interaction as one line in the file:
        for in_tool in in_list:
            # Convert the input tool index to its name:
            in_name = idx2name[in_tool]
            print('{0}\t{0}\tpd\t{1}'.format(out_name, in_name), end="\n", file=file_handle)
        
