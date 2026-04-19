#!/usr/bin/perl

use warnings;
use strict;

my %query_fields;
my $json_result = "{\n";

if (exists $ENV{"QUERY_STRING"}) {
	my $query_string = $ENV{"QUERY_STRING"};
	my @fields = split("&", $query_string);
	for my $i (0 .. $#fields) {
		my ($field_name, $field_value) = split("=", $fields[$i], 2);
		if ($i < $#fields) {
			$json_result .= "\t\"$field_name\": \"$field_value\",\n";
		} else {
			$json_result .= "\t\"$field_name\": \"$field_value\"\n";
		}
	}
}

$json_result .= "}\n";
my $content_length = length($json_result);

print "Status: 200\n";
print "Content-Length: $content_length\n";
print "Content-Type: application/json\n";
print "\n";

print "$json_result";