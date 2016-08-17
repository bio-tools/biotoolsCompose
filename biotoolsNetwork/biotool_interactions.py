import json,urllib.request,codecs
from pprint import pprint

# retrieve json with entire bio.tools data 
url = "https://bio.tools/api/tool"
response = urllib.request.urlopen(url).read().decode("utf-8")
data = json.loads(response)

#with open("bio.tools.json", encoding="utf8") as data_file:
#    data = json.load(data_file)


idx2name = dict()      # Look-up the tool name by its index 
input2tool = dict()    # Look-up all the tools that uses a particular EDAM input format
input2tool2 = dict()    # Look-up all the tools that uses a particular EDAM input data type
input2tool3 = dict()    # Look-up all the tools that uses a particular EDAM input data type | data format
output2tool = dict()   # Look-up all the tools that uses a particular EDAM output format 
output2tool2 = dict()   # Look-up all the tools that uses a particular EDAM output data type
output2tool3 = dict()   # Look-up all the tools that uses a particular EDAM output data type | data format
operations2tool = dict()     # Full EDAM operations for each tool

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
                    if 'dataType' in f_input:
                        curr_type = ""
                        # print(f_input['dataType'])
                        dataType = f_input['dataType']
                        # Extract and keep the EDAM url:
                        if 'uri' in dataType:
                            EDAM_url = dataType['uri']
                            # Get the EDAM format id from the url:
                            EDAM_id = EDAM_url.split('/')[-1]
                            curr_type = EDAM_id
                            # Store the tool index for this EDAM input format:
                            if EDAM_id in input2tool2:
                                input2tool2[EDAM_id].append(idx)
                            else:
                                input2tool2[EDAM_id] = [idx]
                        else: # No url defined
                            pass
                        # Then all the data formats used for the input:
                        if 'dataFormat' in f_input:
                            for dataFormat in f_input['dataFormat']:
                                # Extract and keep the EDAM url:
                                if 'uri' in dataFormat:
                                    EDAM_url = dataFormat['uri']
                                    # Get the EDAM format id from the url:
                                    EDAM_id = EDAM_url.split('/')[-1]
                                    # Store the tool index for this EDAM input format:
                                    combkey = curr_type + "|" + EDAM_id
                                    print(combkey + " " + name)

                                    if EDAM_id in input2tool:
                                        input2tool[EDAM_id].append(idx)
                                    else:
                                        input2tool[EDAM_id] = [idx]
                                    if combkey in input2tool3:
                                        input2tool3[combkey].append(idx)
                                    else:
                                        input2tool3[combkey] = [idx]                                        
                                else: # No url defined
                                    pass
                            else: # No dataFormat or EDAM_url defined
                                pass
                    else: # No dataType defined
                        pass
            else: # No input defined
                pass

            # collect all EDAM operations
            if 'functionName' in function:
                for f_operation in function['functionName']:
                    if 'uri' in f_operation:
                        EDAM_url = f_operation['uri']
                        # Get the EDAM format id from the url:
                        EDAM_id = EDAM_url.split('/')[-1]
                        # Store the operation
                        if idx in operations2tool:
                            operations2tool[idx].append(EDAM_id)
                        else:
                            operations2tool[idx] = [EDAM_id]
                    else: # No url defined
                        # Store empty operation
                        if idx in operations2tool:
                            operations2tool[idx].append("")
                        else:
                            operations2tool[idx] = [""]
            else:
                operations2tool[idx] = [""]
                    

            # Same procedure as with the input defined above:
    if 'function' in tool:
        for function in tool['function']:
            # Now all the inputs of the tool function:
            if 'output' in function:
                for f_output in function['output']:
                    if 'dataType' in f_output:
                        curr_type = ""
                        dataType = f_output['dataType']
                        # Extract and keep the EDAM url:
                        if 'uri' in dataType:
                            EDAM_url = dataType['uri']
                            # Get the EDAM format id from the url:
                            EDAM_id = EDAM_url.split('/')[-1]
                            curr_type = EDAM_id
                            # Store the tool index for this EDAM output format:
                            if EDAM_id in output2tool2:
                                output2tool2[EDAM_id].append(idx)
                            else:
                                output2tool2[EDAM_id] = [idx]
                        else: # No url defined
                            pass
                        # Then all the data formats used for the input:
                        if 'dataFormat' in f_output:
                            for dataFormat in f_output['dataFormat']:
                                # Extract and keep the EDAM url:
                                if 'uri' in dataFormat:
                                    EDAM_url = dataFormat['uri']
                                    # Get the EDAM format id from the url:
                                    EDAM_id = EDAM_url.split('/')[-1]
                                    # Store the tool index for this EDAM input format:
                                    combkey = curr_type + "|" + EDAM_id
                                    # print(combkey + " " + name)
                                    if EDAM_id in output2tool:
                                        output2tool[EDAM_id].append(idx)
                                    else:
                                        output2tool[EDAM_id] = [idx]
                                    if combkey in output2tool3:
                                        output2tool3[combkey].append(idx)
                                    else:
                                        output2tool3[combkey] = [idx]                                        
                                else: # No url defined
                                    pass
                            else: # No dataFormat or EDAM_url defined
                                pass
                    else: # No dataType defined
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

for EDAM_id, outtool_idx in output2tool3.items():
    output2tool3[EDAM_id] = sorted(list(set(outtool_idx)))

for EDAM_id, intool_idx in input2tool3.items():
    input2tool3[EDAM_id] = sorted(list(set(intool_idx)))



# Find the tools with matching output and input:
out_match_in = dict()
# provides EDAM format for each pair of tools
out_edam_in = dict()
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
                for intool in intool_idx:
                    ind = "_".join((str(tool),str(intool)))
                    if ind in out_edam_in:
                        out_edam_in[ind] = sorted(list(set(out_edam_in[ind] + [EDAM_id])))
                    else:
                        out_edam_in[ind] = [EDAM_id]
            else:
                out_match_in[tool] = intool_idx
                for intool in intool_idx:
                    ind = "_".join((str(tool),str(intool)))
                    out_edam_in[ind] = [EDAM_id]
else:
        pass

# Find the tools with matching output and input:
out_match_in3 = dict()
# provides EDAM format for each pair of tools
out_edam_in3 = dict()
# Look through all the EDAM output formats:
for EDAM_id, outtool_idx in output2tool3.items():
    # Is the output format also existing as and input to another tool?:
    if EDAM_id in input2tool3:
        intool_idx = input2tool3[EDAM_id]
        # Now make the connection between the list of input tools and each matching output tool:
        for tool in outtool_idx:
            if tool in out_match_in3:
                # Remove dublicates on the fly, or the dict will grow huge:
                out_match_in3[tool] = sorted(list(set(out_match_in3[tool] + intool_idx)))
                for intool in intool_idx:
                    ind = "_".join((str(tool),str(intool)))
                    if ind in out_edam_in3:
                        out_edam_in3[ind] = sorted(list(set(out_edam_in3[ind] + [EDAM_id])))
                    else:
                        out_edam_in3[ind] = [EDAM_id]
            else:
                out_match_in3[tool] = intool_idx
                for intool in intool_idx:
                    ind = "_".join((str(tool),str(intool)))
                    out_edam_in3[ind] = [EDAM_id]
else:
        pass
   

# Print the results to a Cytoscape tab separated interaction files (1. combined entry per matching EDAM format, 2. individual entries):
with open("biotools.interactions", 'w') as file_handle:
    # Print header:
    print('{0}\t{1}\t{2}\t{3}'.format('tool name/source node', 'tool name/target node', 'interaction format', 'source EDAM operations', 'target EDAM operations'), end="\n", file=file_handle)
    # Go through all tools with output matching an input:
    for out_tool, in_list in out_match_in.items():
        # Convert the output tool index to its name:
        out_name = idx2name[out_tool]
        out_operations = operations2tool[out_tool]
        # Print each interaction as one line in the file:
        for in_tool in in_list:
            # Convert the input tool index to its name:
            in_name = idx2name[in_tool]
            in_operations = operations2tool[in_tool]
            edam_name = out_edam_in["_".join((str(out_tool),str(in_tool)))]
            print('{0}\t{1}\t{2}\t{3}\t{4}'.format(out_name, edam_name,  in_name, out_operations, in_operations), end="\n", file=file_handle)

with open("biotools.interactions.full", 'w') as file_handle:
    # Print header:
    print('{0}\t{1}\t{2}\t{3}'.format('tool name/source node', 'tool name/target node', 'interaction format', 'source EDAM operations', 'target EDAM operations'), end="\n", file=file_handle)
    # Go through all tools with output matching an input:
    for out_tool, in_list in out_match_in.items():
        # Convert the output tool index to its name:
        out_name = idx2name[out_tool]
        out_operations = operations2tool[out_tool]
        # Print each interaction as one line in the file:
        for in_tool in in_list:
            # Convert the input tool index to its name:
            in_name = idx2name[in_tool]
            in_operations = operations2tool[in_tool]
            edam_name = out_edam_in["_".join((str(out_tool),str(in_tool)))]
            for edam_fo in edam_name:
                print('{0}\t{1}\t{2}\t{3}\t{4}'.format(out_name, edam_fo,  in_name, out_operations, in_operations), end="\n", file=file_handle)

with open("biotools.interactions.combEDAM", 'w') as file_handle:
    # Print header:
    print('{0}\t{1}\t{2}\t{3}'.format('tool name/source node', 'interaction format', 'tool name/target node', 'source EDAM operations', 'target EDAM operations'), end="\n", file=file_handle)
    # Go through all tools with output matching an input:
    for out_tool, in_list in out_match_in3.items():
        # Convert the output tool index to its name:
        out_name = idx2name[out_tool]
        out_operations = operations2tool[out_tool]
        # Print each interaction as one line in the file:
        for in_tool in in_list:
            # Convert the input tool index to its name:
            in_name = idx2name[in_tool]
            in_operations = operations2tool[in_tool]
            edam_name = out_edam_in3["_".join((str(out_tool),str(in_tool)))]
            for edam_fo in edam_name:
                print('{0}\t{1}\t{2}\t{3}\t{4}'.format(out_name, edam_fo,  in_name, out_operations, in_operations), end="\n", file=file_handle)

