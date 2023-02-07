$pairs = Get-ChildItem -Recurse -Include *.h,*.cpp | % {
    $f = $_;
    Write-Host "Processing file: $f"

    $fc = gc $f;
    #$allmatches = $fc | sls "\b[a-z]+[A-Z]\w+\b" -CaseSensitive -AllMatches | % { $_.Matches } | ? {
    #    -not $_.Value.StartsWith("v") -and -not $_.Value.StartsWith("x") -and -not $_.Value.StartsWith("pd") -and -not $_.Value.StartsWith("err") -and -not $_.Value.StartsWith("port") -and -not $_.Value.StartsWith("pv") 
    #} | % { $_.Value } | sort | unique;
    $allmatches = $fc | sls "^[^ ]+ [:\w]+::([A-Z]\w+\()" -CaseSensitive -AllMatches | % { $_.Matches.Groups[1].Value } | ? {
        $True
    } | sort | unique;

    $allmatches | % { $old = $_; $new = $old -creplace "[A-Z]",{ "_" + "$_".ToLower()}; $new = $new.Replace("_i_s_r", "_isr").Trim("_"); [pscustomobject]@{"Old"=$old; "New"=$new} }
}

Get-ChildItem -Recurse -Include *.h,*.cpp | % {
    $f = $_;
    Write-Host "Processing file: $f"

    $fc = gc $f;
    $pairs | % { $p = $_; $fc = $fc.Replace($p.Old, $p.New) }
    $fc | Set-Content $f
}